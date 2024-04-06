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

bool UPrototypeAbilitySystemComponent::CanApplyAbility(UPrototypeGameplayAbility* Ability, UAbilitySystemComponent* TargetASC) const
{
	TSubclassOf<UGameplayEffect> AttackWeightGE = Ability->GetAttackWeightGameplayEffect();
	if (!AttackWeightGE) return false;

	if (!TargetASC) return false;

	FGameplayEffectContextHandle ContextHandle = MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	bool bCanApply = TargetASC->CanApplyAttributeModifiers(AttackWeightGE.GetDefaultObject(), 1.0f, ContextHandle);

	return bCanApply;
}

bool UPrototypeAbilitySystemComponent::TryApplyAbility(UPrototypeGameplayAbility* Ability, UAbilitySystemComponent* TargetASC) const
{
	UAbilitySystemComponent* asc = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();
	if (!asc)
	{
		UE_LOG(LogTemp, Warning, TEXT("Owning system component know found: %s"), *Ability->GetName());
		return false;
	}

	TSubclassOf<UGameplayEffect> AttackWeightGE = Ability->GetAttackWeightGameplayEffect();
	if (!AttackWeightGE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack weight effect unknown : %s"), *Ability->GetName());
		return false;
	}

	if (!TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target unknown : %s"), *Ability->GetName());
		return false;
	}

	if (Cast<UPrototypeAbilitySystemComponent>(asc)->CanApplyAbility(Ability, TargetASC))
	{
		FGameplayEffectContextHandle ContextHandle = asc->MakeEffectContext();
		ContextHandle.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle =
			asc->MakeOutgoingSpec(
				*AttackWeightGE,
				1,
				ContextHandle
			);

		//FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(AttackWeightGE->GetClass(), 1.0f, ContextHandle);

		if (SpecHandle.Data.IsValid())
		{
			FActiveGameplayEffectHandle ReturnHandle = asc->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC, asc->GetPredictionKeyForNewAction());
			//FActiveGameplayEffectHandle ReturnHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get(), asc->GetPredictionKeyForNewAction());
			//FActiveGameplayEffectHandle ActiveGameplayEffectHandle = asc->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			if (ReturnHandle.WasSuccessfullyApplied())
			{
				UE_LOG(LogTemp, Warning, TEXT("Attack weight applied successfully : %s"), *Ability->GetName());
				return true;
			}

			return false;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create specHandle : %s"), *Ability->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability attack weight cannont be applied : %s"), *Ability->GetName());
	}

	return false;
}
