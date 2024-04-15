// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Actors/PrototypeBaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Core/Camera/GPCameraComponent.h"
#include "InputAction.h"
#include "Core/Components/GPEnhancedInputComponent.h"
#include "Logging/LogMacros.h"
#include "Core/Components/PrototypeAbilitySystemComponent.h"
#include "Core/Abilities/GPGameplayAbilitySet.h"
#include "Core/Abilities/PrototypeGameplayAbility.h"
#include "Core/Attributes/PrototypeAttributeSet.h"
#include "Core/Tags/PrototypeGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/KismetSystemLibrary.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PrototypeBaseCharacter)

// Sets default values
APrototypeBaseCharacter::APrototypeBaseCharacter(const class FObjectInitializer& ObjectInitializer)
{
	AbilityCameraMode = nullptr;


	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAbilitiesInitialized = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);

	CameraComponent = CreateDefaultSubobject<UGPCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
	CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &APrototypeBaseCharacter::DetermineCameraMode);

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
	if (IsValid(AbilitySystemComponent)) {
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetHealthAttribute()).AddUObject(this, &APrototypeBaseCharacter::OnHealthChangedInternal);
	}
}

void APrototypeBaseCharacter::Landed(const FHitResult& Hit)
{
}

void APrototypeBaseCharacter::NotifyJumpApex()
{
}

TSubclassOf<UGPCameraMode> APrototypeBaseCharacter::DetermineCameraMode() const
{
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}

	if (CameraComponent)
	{
		return CameraComponent->DefaultCameraMode;
	}

	return nullptr;
}

UAbilitySystemComponent* APrototypeBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void APrototypeBaseCharacter::AddStartupGameplayAbilities()
{
	SetRole(ROLE_Authority);

	if (GetLocalRole() != ROLE_Authority || bAbilitiesInitialized) return;

	check(AbilitySystemComponent);

	for (TObjectPtr<UGPGameplayAbilitySet>& StartupAbility : GameplayAbilitySets)
	{
		if (StartupAbility)
		{
			StartupAbility->GiveToAbilitySystem(AbilitySystemComponent, &GrantedHandles, nullptr);
		}
	}

	// Now Apply Passives
	for (const TSubclassOf<UGameplayEffect>& GameplayEffect : PassiveGameplayEffects)
	{
		// FGameplayEffectContextHandle: Wrapper that allows effect to be polymorphic and replicated over network.
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle NewHandle = 
			AbilitySystemComponent->MakeOutgoingSpec(
				GameplayEffect,
				1,
				EffectContext
			);

		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGameplayEffectHandle =
				AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
					*NewHandle.Data.Get(), 
					AbilitySystemComponent
				);
		}
	}

	bAbilitiesInitialized = true;
}

void APrototypeBaseCharacter::AddCharacterAbilityWithInput(TSubclassOf<UPrototypeGameplayAbility> Ability)
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent) return;

	if (Ability)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(
				Ability,
				Ability.GetDefaultObject()->GetAbilityLevel(),
				GetTypeHash(Ability.GetDefaultObject()->AbilityInputID = *InputConfig->FindGameplayTagForAbility(Ability, true)),
				this
			));
	}

	UE_LOG(LogTemp, Warning, TEXT("Ability Name : %s"), *Ability.GetDefaultObject()->GetName());
}

void APrototypeBaseCharacter::RemoveCharacterGameplayAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent) return;
	
	GrantedHandles.TakeFromAbilitySystem(AbilitySystemComponent);

	//TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;

	//for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	//{
	//	if ((Spec.SourceObject == this) && GameplayAbilities.Contains(Spec.Ability->GetClass())) {
	//		AbilitiesToRemove.Add(Spec.Handle);
	//	}
	//}

	//for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	//{
	//	AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	//}
}

void APrototypeBaseCharacter::RemoveCharacterGameplayAbility(const FGameplayAbilitySpecHandle& AbilitySpecHandle)
{
	AbilitySystemComponent->ClearAbility(AbilitySpecHandle);
}

void APrototypeBaseCharacter::InitializeAttributes()
{
}

void APrototypeBaseCharacter::InitializeHealth(float health)
{
	if (Attributes)
	{
		Attributes->InitHealth(health);
		Attributes->InitMaxHealth(health);
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("Attribute Set Not Initialized!"));
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

void APrototypeBaseCharacter::CancelAbilityWithTags(const FGameplayTagContainer CancelWithTags) {
	AbilitySystemComponent->CancelAbilities(&CancelWithTags);
}

float APrototypeBaseCharacter::GetHealth() const
{
	if (Attributes)
	{
		return Attributes->GetHealth();
	}

	UE_LOG(LogTemp, Warning, TEXT("Attribute Set Not Initialized!"));
	return 0.0f;
}

float APrototypeBaseCharacter::GetMaxHealth() const
{
	if (Attributes)
	{
		return Attributes->GetMaxHealth();
	}

	UE_LOG(LogTemp, Warning, TEXT("Attribute Set Not Initialized!"));
	return 0.0f;
}

void APrototypeBaseCharacter::PossessedBy(AController* NewController)
{
	UE_LOG(LogTemp, Warning, TEXT("Pawn is possessed by: %s"), *NewController->GetHumanReadableName());
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AddStartupGameplayAbilities();
	}

	SetOwner(NewController);
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

	if (InputConfig == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("InputConfig is null"));
		return;
	}

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	for (const FTaggedInputAction& Action : InputConfig->TaggedInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("InputAction bound to InputAction %s"), *Action.InputTag.ToString());

			EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Triggered, this, &APrototypeBaseCharacter::AbilityInputPressed, Action.InputTag);
			EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Completed, this, &APrototypeBaseCharacter::AbilityInputReleased, Action.InputTag);
		}
	}
}

void APrototypeBaseCharacter::SetAbilityCameraMode(TSubclassOf<UGPCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (CameraMode)
	{
		AbilityCameraMode = CameraMode;
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
	}
}

void APrototypeBaseCharacter::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		AbilityCameraMode = nullptr;
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
	}
}

void APrototypeBaseCharacter::AbilityInputPressed(const FGameplayTag InputTag)
{
	AbilitySystemComponent->AbilityLocalInputPressed(GetTypeHash(InputTag));
}

void APrototypeBaseCharacter::AbilityInputReleased(const FGameplayTag InputTag)
{
	AbilitySystemComponent->AbilityLocalInputReleased(GetTypeHash(InputTag));
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
		static_cast<int32>(EAbilityInputID::Confirm),
		static_cast<int32>(EAbilityInputID::Cancel)
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

void APrototypeBaseCharacter::OnHealthChangedInternal(const FOnAttributeChangeData& Data) {
	OnHealthChanged1(Data.OldValue, Data.NewValue);
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

void APrototypeBaseCharacter::Input_Fire(const EAbilityInputID AbilityInputTag)
{
	OnPrimaryAction();
}

void APrototypeBaseCharacter::EquipWeapon()
{
	for (const FGPEquipmentActorToSpawn& SpawnInfo : EquipmentToSpawn)
	{
		AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, this);
		NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
		NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
		NewActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket); 
		DestroyEquipmentActors();
		SpawnedEquipment = NewActor;
	}
}

void APrototypeBaseCharacter::DestroyEquipmentActors()
{
	if (SpawnedEquipment)
	{
		SpawnedEquipment->Destroy();
	}
}

APawn* APrototypeBaseCharacter::GetPawn() const
{
	return Cast<APawn>(GetOuter());
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