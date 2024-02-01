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


//Hit boxes related---------------------------------------------------------------------------------------------------

TArray<FPoCharacterHitBox> UPoCharacterHitBoxes::GetMatchingHitBoxes(TArray<FGameplayTag> Tags) {
	TArray<FPoCharacterHitBox> Res;
	for (auto GameplayTag : Tags) {
		FPoCharacterHitBox* HitBox = HitBoxes.Find(GameplayTag);
		if (HitBox) {
			Res.Add(*HitBox);
		}
	}
	return Res;
}

bool UPoCharacterHitBoxes::GetHitBox(FGameplayTag Tag, FPoCharacterHitBox& Out_HitBox) {
	FPoCharacterHitBox* HitBox = HitBoxes.Find(Tag);
	if (HitBox) {
		Out_HitBox = *HitBox;
		return true;
	}
	return false;
}