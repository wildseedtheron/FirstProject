#include "Core/Abilities/ObkCombatAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

//The PostGameplayEffectExecute function handles any logic that should happen after a GameplayEffect is applied to the Ability System Component that owns this AttributeSet. In effect it "listens" for changes to happen.
void UObkCombatAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) {
	Super::PostGameplayEffectExecute(Data);

	//Clamping Health based on Max Health.
	if (Data.EvaluatedData.Attribute == GetHealthAttribute()) {
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
}

float UObkCombatAttributeSet::GetHealth() const {
	return FMath::Max(Health.GetCurrentValue(), 0.0f);
}

void UObkCombatAttributeSet::SetHealth(float NewVal) {
    NewVal = FMath::Max(NewVal, 0.0f);
    UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
    if (ensure(ASC)) { ASC->SetNumericAttributeBase(GetHealthAttribute(), NewVal); }
}