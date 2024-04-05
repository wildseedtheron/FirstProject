// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Abilities/PrototypeGameplayAbility.h"
#include "AbilitySystemLog.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Core/Abilities/GPAbilitySystemGlobals.h"
#include "Core/Actors/PrototypeBaseCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PrototypeGameplayAbility)

#define ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(FunctionName, ReturnValue)																				\
{																																						\
	if (!ensure(IsInstantiated()))																														\
	{																																					\
		ABILITY_LOG(Error, TEXT("%s: " #FunctionName " cannot be called on a non-instanced ability. Check the instancing policy."), *GetPathName());	\
		return ReturnValue;																																\
	}																																					\
}

UPrototypeGameplayAbility::UPrototypeGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActiveCameraMode = nullptr;
}

bool UPrototypeGameplayAbility::CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags)
{
	if (!Super::CommitCheck(Handle, ActorInfo, ActivationInfo, OUT OptionalRelevantTags)) return false;

	UGPAbilitySystemGlobals* AbilitySystemGlobals = Cast<UGPAbilitySystemGlobals>(&UAbilitySystemGlobals::Get());

	if (!AbilitySystemGlobals && !CheckAttackWeight(Handle, ActorInfo, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UPrototypeGameplayAbility::CommitExecute(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::CommitExecute(Handle, ActorInfo, ActivationInfo);

	ApplyAttackWeight(Handle, ActorInfo, ActivationInfo);
}

bool UPrototypeGameplayAbility::CheckAttackWeightWithTags(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, FGameplayTagContainer& OptionalRelevantTagsOUT) const
{
	return CheckAttackWeight(Handle, &ActorInfo, &OptionalRelevantTagsOUT);
}

bool UPrototypeGameplayAbility::CheckAttackWeightSimple(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const
{
	return CheckAttackWeight(Handle, &ActorInfo);
}

bool UPrototypeGameplayAbility::CheckAttackWeight(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	UGameplayEffect* AttackWeightGE = GetAttackWeightGameplayEffect();
	if (AttackWeightGE)
	{
		UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
		check(AbilitySystemComponent != nullptr);
		if (!AbilitySystemComponent->CanApplyAttributeModifiers(AttackWeightGE, GetAbilityLevel(Handle, ActorInfo), MakeEffectContext(Handle, ActorInfo)))
		{
			// Cast the singleton AbilitySystemGlobals to your custom class
			UGPAbilitySystemGlobals* Globals = Cast<UGPAbilitySystemGlobals>(&UAbilitySystemGlobals::Get());
			if (!Globals)
			{
				// Handle the error if the cast fails
				UE_LOG(LogTemp, Warning, TEXT("Failed to cast AbilitySystemGlobals to UGPAbilitySystemGlobals."));
				return false;
			}

			// Now access custom global variable
			const FGameplayTag& AttackWeightTag = Globals->ActivateFailAttackWeightTag; // Note: using the Tag, not Name

			if (OptionalRelevantTags && AttackWeightTag.IsValid())
			{
				OptionalRelevantTags->AddTag(AttackWeightTag);
			}
			return false;
		}
	}
	return true;
}


void UPrototypeGameplayAbility::ApplyAttackWeight(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* AttackWeightGE = GetAttackWeightGameplayEffect();
	if (AttackWeightGE)
	{
		ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, AttackWeightGE, GetAbilityLevel(Handle, ActorInfo));
	}
}

UGameplayEffect* UPrototypeGameplayAbility::GetAttackWeightGameplayEffect() const
{
	if (AttackWeightGameplayEffectClass)
	{
		return AttackWeightGameplayEffectClass->GetDefaultObject<UGameplayEffect>();
	}
	else
	{
		return nullptr;
	}
}

void UPrototypeGameplayAbility::SetCameraMode(TSubclassOf<UGPCameraMode> CameraMode)
{
	ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(SetCameraMode, );
	if (APrototypeBaseCharacter* OwnerActor = Cast<APrototypeBaseCharacter>(GetActorInfo().AvatarActor))
	{
		OwnerActor->SetAbilityCameraMode(CameraMode, CurrentSpecHandle);
		ActiveCameraMode = CameraMode;
	}
}

void UPrototypeGameplayAbility::ClearCameraMode()
{

	if (ActiveCameraMode)
	{
		if (APrototypeBaseCharacter* OwnerActor = Cast<APrototypeBaseCharacter>(GetActorInfo().AvatarActor))
		{
			OwnerActor->ClearAbilityCameraMode(CurrentSpecHandle);
		}

		ActiveCameraMode = nullptr;
	}
}

FGameplayAbilitySpecHandle UPrototypeGameplayAbility::GetSpecHandle()
{
	return GetCurrentAbilitySpecHandle();
}
