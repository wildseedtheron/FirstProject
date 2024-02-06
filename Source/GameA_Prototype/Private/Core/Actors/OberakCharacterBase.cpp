#include "Core/Actors/OberakCharacterBase.h"
#include "AbilitySystemComponent.h"

AOberakCharacterBase::AOberakCharacterBase() {
	PrimaryActorTick.bCanEverTick = true;
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));
}

void AOberakCharacterBase::BeginPlay() {
	Super::BeginPlay();
	if (IsValid(AbilitySystemComponent)) {
		AttributeSet = AbilitySystemComponent->GetSet<UObkCombatAttributeSet>();
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &AOberakCharacterBase::OnHealthChangedInternal);
	}
}

void AOberakCharacterBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AOberakCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AOberakCharacterBase::CancelAbilityWithTags(const FGameplayTagContainer CancelWithTags) {
	AbilitySystemComponent->CancelAbilities(&CancelWithTags);
}



//Health
void AOberakCharacterBase::OnHealthChangedInternal(const FOnAttributeChangeData& Data) {
	OnHealthChanged(Data.OldValue, Data.NewValue);
}
float AOberakCharacterBase::GetHealth() const {
	if (IsValid(AttributeSet)) { return AttributeSet->GetHealth(); }
	return -1.0f;
}
float AOberakCharacterBase::GetMaxHealth() const {
	if (IsValid(AttributeSet)) { return AttributeSet->GetMaxHealth(); }
	return 0.0f;
}