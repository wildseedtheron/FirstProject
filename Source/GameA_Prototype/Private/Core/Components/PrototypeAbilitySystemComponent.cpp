// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Components/PrototypeAbilitySystemComponent.h"
#include "Core/Abilities/PrototypeGameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PrototypeAbilitySystemComponent)

UPrototypeAbilitySystemComponent::UPrototypeAbilitySystemComponent() {}


void UPrototypeAbilitySystemComponent::AbilityInputTagPressed(int32 InputID)
{
	// Consume the input if this InputID is overloaded with GenericConfirm/Cancel and the GenericConfim/Cancel callback is bound
	if (IsGenericConfirmInputBound(InputID))
	{
		LocalInputConfirm();
		return;
	}

	if (IsGenericCancelInputBound(InputID))
	{
		LocalInputCancel();
		return;
	}

	// ---------------------------------------------------------

	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.InputID == InputID)
		{
			if (Spec.Ability)
			{
				Spec.InputPressed = true;
				if (Spec.IsActive())
				{
					if (Spec.Ability->bReplicateInputDirectly && IsOwnerActorAuthoritative() == false)
					{
						ServerSetInputPressed(Spec.Handle);
					}

					AbilitySpecInputPressed(Spec);

					// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
				}
				else
				{
					// Ability is not active, so try to activate it
					TryActivateAbility(Spec.Handle);
				}
			}
		}
	}
}

void UPrototypeAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag) {
	FGameplayTagContainer tagContainer;
	tagContainer.AddTag(InputTag);
	bool result = TryActivateAbilitiesByTag(tagContainer);
	UE_LOG(LogTemp, Warning, TEXT("TryActivateAbilitiesByTag: %s"), (result ? TEXT("true") : TEXT("false")));
}

void UPrototypeAbilitySystemComponent::ReceiveDamage(UPrototypeAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	if (SourceASC)
	{
		ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
	}
}

bool UPrototypeAbilitySystemComponent::CanApplyAbility(UPrototypeGameplayAbility* Ability, const AActor* SourceActor, const AActor* TargetActor) const
{
	UGameplayEffect* AttackWeightGE = Ability->GetAttackWeightGameplayEffect();
	if (!AttackWeightGE) return false;

	UAbilitySystemComponent* SourceAbilitySystemComponent = SourceActor->FindComponentByClass<UAbilitySystemComponent>();
	if (!SourceAbilitySystemComponent) return false;

	FGameplayAbilityActorInfo* SourceActorInfo = SourceAbilitySystemComponent->AbilityActorInfo.Get();
	if (!SourceActorInfo) return false;

	UAbilitySystemComponent* TargetAbilitySystemComponent = TargetActor->FindComponentByClass<UAbilitySystemComponent>();
	if (!TargetAbilitySystemComponent) return false;

	float EffectLevel = 1.0f; // The level of the effect, adjust as needed

	FGameplayEffectContextHandle ContextHandle = SourceAbilitySystemComponent->MakeEffectContext();

	bool bCanApply = TargetAbilitySystemComponent->CanApplyAttributeModifiers(AttackWeightGE, 0, ContextHandle);

	if (bCanApply)
	{
		FGameplayEffectSpecHandle SpecHandle = SourceAbilitySystemComponent->MakeOutgoingSpec(AttackWeightGE->GetClass(), 1.0f, ContextHandle);

		if (SpecHandle.IsValid())
		{
			SourceAbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetAbilitySystemComponent);
			return true;
		}
	}

	return false;
}
