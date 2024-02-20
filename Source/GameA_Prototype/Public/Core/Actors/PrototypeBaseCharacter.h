// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/PrototypeData.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "EnhancedInputSubsystems.h"
#include "PrototypeBaseCharacter.generated.h"

class UPrototypeAbilitySystemComponent;
class UGPCameraComponent;
class UGPCameraMode;
class UPrototypeGameplayAbility;
class UPrototypeAttributeSet;
class UGameplayEffect;
class UInputComponent;
class UGPInputConfig;
struct FInputActionValue;

// Declaration of the delegate that will be called when the Primary Action is triggered
// It is declared as dynamic so it can be accessed also in Blueprints
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

UCLASS()
class GAMEA_PROTOTYPE_API APrototypeBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APrototypeBaseCharacter(const class FObjectInitializer& ObjectInitializer);

#pragma region Class Overrides
protected:
	virtual void BeginPlay() override; // Called when the game starts or when spawned
	virtual void Landed(const FHitResult& Hit) override;
	virtual void NotifyJumpApex() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
public:
	virtual void Tick(float DeltaTime) override; // Called every frame
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override; // Called to bind functionality to input
#pragma endregion

#pragma region Ability System
protected:

	TSubclassOf<UGPCameraMode> DetermineCameraMode() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void AddStartupGameplayAbilities();
	virtual void AddCharacterAbilityWithInput(TSubclassOf<UPrototypeGameplayAbility>& Ability);
	void RemoveCharacterGameplayAbilities();
	void RemoveCharacterGameplayAbility(const FGameplayAbilitySpecHandle& AbilitySpecHandle);
	virtual void InitializeAttributes();
	virtual void InitializeHealth(float health);

	UEnhancedInputLocalPlayerSubsystem* GetInputSubsystem() const;

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void CancelAbilityWithTags(const FGameplayTagContainer CancelWithTags);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealth() const;

	/// <summary>
	/// Sets the default attribute values on init.
	/// </summary>
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UPrototypeGameplayAbility>> GameplayAbilities;

	UPROPERTY()
	uint8 bAbilitiesInitialized : 1;

	/// <summary>
	/// Called when character takes damage, which may have killed them.
	/// </summary>
	/// <param name="DamageAmount">Amount of damage that was done, not clamped based on current health.</param>
	/// <param name="HitInfo">The hit info that generated this damage.</param>
	/// <param name="DamageTags">The gameplay tags of the event that did the damage.</param>
	/// <param name="InstigatorCharacter">The character that initiated this damage.</param>
	/// <param name="DamageOwner">The actual actor that did the damage, might be a weapon or projectile.</param>
	UFUNCTION(BlueprintImplementableEvent)
	void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags,
		APrototypeBaseCharacter* InstigatorCharacter, AActor* DamageOwner);

	/// <summary>
	/// Called when health is changed, either from healing or from being damaged.
	/// For damage this is called in addition to OnDamaged/OnKilled
	/// </summary>
	/// <param name="DeltaValue">Change in health value, positive for heal, negative for damage. If 0 the delta is unknown.</param>
	/// <param name="EventTags">The gameplay tags of the event that changed health.</param>
	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags,
		APrototypeBaseCharacter* InstigatorCharacter, AActor* DamageOwner);

	virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	//virtual void HandleFireWeaponPressed();

	friend UPrototypeAttributeSet;
#pragma endregion

#pragma region Ability Component Property
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGPCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPrototypeAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Abilities")
	TObjectPtr<UPrototypeAttributeSet> Attributes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Abilities")
	TSubclassOf<UGameplayEffect> DefaultAttributes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Abilities")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float TurnRateGamepad;

	///** Delegate to whom anyone can subscribe to receive this event */
	//UPROPERTY(BlueprintAssignable, Category = "Interaction")
	//FOnUseItem OnUseItem;

	//UPROPERTY(EditDefaultsOnly,  Category="Input|Binding")
	//FAbilityInputBinding AbilityInputBindings;
#pragma endregion

#pragma region Input Actions
protected:
	/** Fires a projectile. */
	void OnPrimaryAction();

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

public:

	UFUNCTION(BlueprintCallable, Category = "Camera")
	/** Overrides the camera from an active gameplay ability */
	void SetAbilityCameraMode(TSubclassOf<UGPCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle);

	/** Clears the camera override if it is set */
	void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle);

	void AbilityInputPressed(const FGameplayTag InputTag);

	void AbilityInputReleased(const FGameplayTag InputTag);

	/** Handles moving forward/backward */
	void Input_Move(const FInputActionValue& InputActionValue);

	/** Handles mouse and stick look */
	void Input_Look(const FInputActionValue& InputActionValue);

	/** Handles Pew Pew */
	void Input_Fire(const EAbilityInputID AbilityInputID);



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Controls|Input Actions")
	UInputAction* Fire;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Controls|Input Actions")
	UInputAction* Jumps;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UGPInputConfig* InputConfig;
#pragma endregion


protected:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnDeath();

	/** Camera mode set by an ability. */
	UPROPERTY()
	TSubclassOf<UGPCameraMode> AbilityCameraMode;

	/** Spec handle for the last ability to set a camera mode. */
	FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;
};
