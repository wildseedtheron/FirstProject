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
};
