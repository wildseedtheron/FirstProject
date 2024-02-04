#include "Core/Actors/OberakCharacterBase.h"
#include "AbilitySystemComponent.h"

AOberakCharacterBase::AOberakCharacterBase() {
	PrimaryActorTick.bCanEverTick = true;
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));
}

void AOberakCharacterBase::BeginPlay() {
	Super::BeginPlay();
}

void AOberakCharacterBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AOberakCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AOberakCharacterBase::CancelAbilityWithTags(const FGameplayTagContainer CancelWithTags) {
	//The Ability System Component has multiple ways of cancelling and activating abilities.
	//Using tags provides a generic means of categorizing different types of abilities without
	//knowing what specific ability is active.
	AbilitySystemComponent->CancelAbilities(&CancelWithTags);
}

