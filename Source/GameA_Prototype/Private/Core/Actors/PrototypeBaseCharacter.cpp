// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Actors/PrototypeBaseCharacter.h"
#include "InputAction.h"
#include "Core/Components/GPEnhancedInputComponent.h"
#include "Logging/LogMacros.h"
#include "Core/Components/PrototypeAbilitySystemComponent.h"
#include "Core/Abilities/PrototypeGameplayAbility.h"
#include "Core/Attributes/PrototypeAttributeSet.h"
#include "Core/Tags/PrototypeGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/KismetSystemLibrary.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PrototypeBaseCharacter)

// Sets default values
APrototypeBaseCharacter::APrototypeBaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAbilitiesInitialized = false;

	AbilitySystemComponent = CreateDefaultSubobject<UPrototypeAbilitySystemComponent>(TEXT("Ability System"));

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetIsReplicated(true);
		AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	}

	Attributes = CreateDefaultSubobject<UPrototypeAttributeSet>(TEXT("Attributes"));
}

// Called when the game starts or when spawned
void APrototypeBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APrototypeBaseCharacter::Landed(const FHitResult& Hit)
{
}

void APrototypeBaseCharacter::NotifyJumpApex()
{
}

UAbilitySystemComponent* APrototypeBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void APrototypeBaseCharacter::AddStartupGameplayAbilities()
{
	SetRole(ROLE_Authority);

	if (GetLocalRole() != ROLE_Authority || bAbilitiesInitialized) return;

	check(AbilitySystemComponent);

	// Grant Abilities only on the server.
	for (TSubclassOf<UPrototypeGameplayAbility>& StartupAbility : GameplayAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(
				StartupAbility, 
				StartupAbility.GetDefaultObject()->GetAbilityLevel(),
				static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID),
				this
			));
		UE_LOG(LogTemp, Warning, TEXT("Ability Name : %s"), *StartupAbility.GetDefaultObject()->GetName());
	}

	// Now Apply Passives
	for (const TSubclassOf<UGameplayEffect>& GameplayEffect : PassiveGameplayEffects)
	{
		// FGameplayEffectContextHandle: Wrapper that allows effect to be polymorphic and replicated over network.
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(
			GameplayEffect, 1, EffectContext);

		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGameplayEffectHandle =
				AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
					*NewHandle.Data.Get(), AbilitySystemComponent);
		}
	}

	UKismetSystemLibrary::PrintString(this, "We Are Pressing Our Input");
	bAbilitiesInitialized = true;
}

UEnhancedInputLocalPlayerSubsystem* APrototypeBaseCharacter::GetInputSubsystem() const
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = nullptr;

	// Make sure that we have a valid PlayerController.
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController) 
	{ 
		Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()); 
	}

	return Subsystem;
}

void APrototypeBaseCharacter::PossessedBy(AController* NewController)
{
	UE_LOG(LogTemp, Warning, TEXT("Pawn is possessed by: %s"), NewController);
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AddStartupGameplayAbilities();
	}
}

// Called every frame
void APrototypeBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APrototypeBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("Setting up player input component."));
	//Make sure to set your input component class in the InputSettings->DefaultClasses
	check(PlayerInputComponent);


	if (InputConfig != nullptr)
	{
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
		for (const FTaggedInputAction& Action : InputConfig->TaggedInputActions)
		{
			if (Action.InputAction && Action.InputTag.IsValid())
			{
				if (*Action.InputTag.ToString() == FString(TEXT("InputTag.Ability.Jump")))
				{
					EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Triggered, this, &APrototypeBaseCharacter::Start_Jump);
					EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Completed, this, &APrototypeBaseCharacter::Stop_Jump);
					UE_LOG(LogTemp, Warning, TEXT("InputAction Name : %s"), *Action.InputTag.ToString());
				}

				if (*Action.InputTag.ToString() == FString(TEXT("InputTag.Ability.Punch")))
				{
					EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Triggered, this, &APrototypeBaseCharacter::Start_Punch);
					EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Completed, this, &APrototypeBaseCharacter::Stop_Punch);
					UE_LOG(LogTemp, Warning, TEXT("InputAction Name : %s"), *Action.InputTag.ToString());
				}

				if (*Action.InputTag.ToString() == FString(TEXT("InputTag.Ability.Kick")))
				{
					EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Triggered, this, &APrototypeBaseCharacter::Start_Kick);
					EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Completed, this, &APrototypeBaseCharacter::Stop_Kick);
					UE_LOG(LogTemp, Warning, TEXT("InputAction Name : %s"), *Action.InputTag.ToString());
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InputConfig is null"));
	}
}

void APrototypeBaseCharacter::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	//OnUseItem.Broadcast();
}

void APrototypeBaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AbilitySystemComponent || InputComponent) return;

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	const FGameplayAbilityInputBinds Binds(
		"Confirm",
		"Cancel",
		"PrototypeAbilityInputID",
		static_cast<int32>(EPrototypeAbilityInputID::Confirm),
		static_cast<int32>(EPrototypeAbilityInputID::Cancel)
	);

	AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
}

void APrototypeBaseCharacter::HandleDamage(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags, APrototypeBaseCharacter* InstigatorCharacter, AActor* DamageOwner)
{
	OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageOwner);
}

void APrototypeBaseCharacter::HandleHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized)
	{
		OnHealthChanged(DeltaValue, EventTags);
	}
}

void APrototypeBaseCharacter::SendLocalInputToASC(bool bIsPressed, const EPrototypeAbilityInputID AbilityInputID)
{
	if (!AbilitySystemComponent) { UKismetSystemLibrary::PrintString(this, "We dont have a valid ASC"); return; }

	if (bIsPressed)
	{
		UKismetSystemLibrary::PrintString(this, "We Are Pressing Our Input");
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(AbilityInputID));
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, "We Are Releasing Our Input");
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(AbilityInputID));
	}
}

void APrototypeBaseCharacter::Start_Jump(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(true, EPrototypeAbilityInputID::Jump);
}

void APrototypeBaseCharacter::Stop_Jump(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(false, EPrototypeAbilityInputID::Jump);
}

void APrototypeBaseCharacter::Start_Punch(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(true, EPrototypeAbilityInputID::Punch);
}

void APrototypeBaseCharacter::Stop_Punch(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(false, EPrototypeAbilityInputID::Punch);
}

void APrototypeBaseCharacter::Start_Kick(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(true, EPrototypeAbilityInputID::Kick);
}

void APrototypeBaseCharacter::Stop_Kick(const FInputActionValue& InputActionValue)
{
	SendLocalInputToASC(false, EPrototypeAbilityInputID::Kick);
}

void APrototypeBaseCharacter::HandleFireWeaponPressed()
{
	SendLocalInputToASC(true, EPrototypeAbilityInputID::Fire);
}

void APrototypeBaseCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	if (Controller != nullptr)
	{
		const FVector2D MoveValue = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (MoveValue.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			AddMovementInput(MovementDirection, MoveValue.X);
		}

		if (MoveValue.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			AddMovementInput(MovementDirection, MoveValue.Y);
		}
	}
}

void APrototypeBaseCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	if (Controller != nullptr)
	{
		const FVector2D LookValue = InputActionValue.Get<FVector2D>();

		if (LookValue.X != 0.0f)
		{
			TurnAtRate(LookValue.X);
		}

		if (LookValue.Y != 0.0f)
		{
			LookUpAtRate(LookValue.Y);
		}
	}
}

void APrototypeBaseCharacter::Input_Fire(const FInputActionValue& InputActionValue)
{
	OnPrimaryAction();
}

void APrototypeBaseCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void APrototypeBaseCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}