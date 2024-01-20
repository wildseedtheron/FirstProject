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
