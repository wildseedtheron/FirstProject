// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PrototypeData.h"
#include "Core/GPInputConfig.h"
#include "GPLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GPInputConfig)

const UInputAction* UGPInputConfig::FindInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FTaggedInputAction& TaggedInputAction : TaggedInputActions)
	{
		if (TaggedInputAction.InputAction && TaggedInputAction.InputTag == InputTag)
		{
			return TaggedInputAction.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogLyra, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}