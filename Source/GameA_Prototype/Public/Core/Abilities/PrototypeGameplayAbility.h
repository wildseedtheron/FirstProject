// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/PrototypeData.h"
#include "Abilities/GameplayAbility.h"
//#include "InputMappingContext.h"
//#include "InputAction.h"
#include "PrototypeGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GAMEA_PROTOTYPE_API UPrototypeGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPrototypeGameplayAbility();

	// Abilities will activate when input is pressed
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	EPrototypeAbilityInputID AbilityInputID{ EPrototypeAbilityInputID::None };

	// Abilities will activate when input is pressed
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input")
	FGameplayTag InputTag{ FGameplayTag::EmptyTag };
};
