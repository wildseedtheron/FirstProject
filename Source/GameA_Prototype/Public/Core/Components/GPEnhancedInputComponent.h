// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnhancedInputComponent.h"
#include "Core/GPInputConfig.h"

#include "GPEnhancedInputComponent.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
class UObject;

/**
 * GPInputComponent
 *
 *	Component used to manage input mappings and bindings using an input config data asset.
 */
UCLASS(Config = Input)
class GAMEA_PROTOTYPE_API UGPEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UGPEnhancedInputComponent(const FObjectInitializer& ObjectInitializer);

	void AddInputMappings(const UGPInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	void RemoveInputMappings(const UGPInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UGPInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	void RemoveBinds(TArray<uint32>& BindHandles);
};

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UGPEnhancedInputComponent::BindAbilityActions(const UGPInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	UE_LOG(LogTemp, Warning, TEXT("Binding Ability Actions"));

	check(InputConfig);

	for (const FTaggedInputAction& Action : InputConfig->TaggedInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				//BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc).GetHandle();
				//UE_LOG(LogTemp, Warning, TEXT("Null ref"));
			}

			if (ReleasedFunc)
			{
				//BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
				//BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
				UE_LOG(LogTemp, Warning, TEXT("ReleasedFunc Bound"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Input Tag Is InValid!"));
		}
	}
}
