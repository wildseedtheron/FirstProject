// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/PrototypeData.h"
#include "Abilities/GameplayAbility.h"
#include "Core/Camera/GPCameraMode.h"
#include "PrototypeGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GAMEA_PROTOTYPE_API UPrototypeGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPrototypeGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** The last chance to fail before committing, this will usually be the same as CanActivateAbility. Some abilities may need to do extra checks here if they are consuming extra stuff in CommitExecute */
	virtual bool CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr);

	/** Does the commit atomically (consume resources, do cooldowns, etc) */
	virtual void CommitExecute(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool CheckAttackWeightWithTags(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, FGameplayTagContainer& OptionalRelevantTagsOUT) const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool CheckAttackWeightSimple(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const;
	/** Checks attack weight. returns true if target has enough attack weight for the ability. False if not */
	bool CheckAttackWeight(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;

	/** Applies the ability's cost to the target */
	virtual void ApplyAttackWeight(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const;

	/** Returns the gameplay effect used to apply cost */
	virtual UGameplayEffect* GetAttackWeightGameplayEffect() const;

	/** Apply chosen ability weight to target. */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	float GetAbilityAttackWeight() const;

	// Sets the ability's camera mode.
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void SetCameraMode(TSubclassOf<UGPCameraMode> CameraMode);

	// Clears the ability's camera mode.  Automatically called if needed when the ability ends.
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void ClearCameraMode();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	FGameplayAbilitySpecHandle GetSpecHandle();

	// Abilities will activate when input is pressed
	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	FGameplayTag AbilityInputID;

	// Current camera mode set by the ability.
	TSubclassOf<UGPCameraMode> ActiveCameraMode;

protected:
	/** This GameplayEffect represents the attack weight of the ability. It will be applied when the ability is committed. */
	UPROPERTY(EditDefaultsOnly, Category = "Attack Weight")
	TSubclassOf<class UGameplayEffect> AttackWeightGameplayEffectClass;

	/** This GameplayEffect represents the attack weight of the ability. It will be applied when the ability is committed. */
	UPROPERTY(EditDefaultsOnly, Category = "Attack Weight")
	float AttackWeight;

	/** This GameplayEffect represents the attack weight of the ability. It will be applied when the ability is committed. */
	UPROPERTY(EditDefaultsOnly, Category = "Attack Weight")
	FSetByCallerFloat AttackDuration;
};
