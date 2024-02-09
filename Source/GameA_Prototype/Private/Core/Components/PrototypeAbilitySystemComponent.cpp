// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Components/PrototypeAbilitySystemComponent.h"
#include "Core/Abilities/PrototypeGameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PrototypeGameplayAbility)

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
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}
