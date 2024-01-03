// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PrototypeAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class GAMEA_PROTOTYPE_API UPrototypeAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	UPrototypeAbilitySystemComponent();

	void AbilityInputTagPressed(const FGameplayTag& InputTag, int32 InputID);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

protected:
	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
};
