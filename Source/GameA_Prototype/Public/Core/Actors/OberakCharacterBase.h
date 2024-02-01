#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "OberakCharacterBase.generated.h"

UCLASS()
class GAMEA_PROTOTYPE_API AOberakCharacterBase : public ACharacter, public IAbilitySystemInterface {
	GENERATED_BODY()

public:
	AOberakCharacterBase();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	class UAbilitySystemComponent* AbilitySystemComponent;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
};



//Hit boxes related---------------------------------------------------------------------------------------------------


USTRUCT(BlueprintType)
struct FPoCharacterHitBox {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName SocketName = "R_wrist";
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float Radius = 25.0f;
};

USTRUCT(BlueprintType)
struct FPoCharacterHitBoxContainer {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray <FPoCharacterHitBox> HitBoxArray;
};

UCLASS(BlueprintType)
class GAMEA_PROTOTYPE_API UPoCharacterHitBoxes : public UDataAsset {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, FPoCharacterHitBox> HitBoxes;
	UFUNCTION(BlueprintPure)
	TArray<FPoCharacterHitBox> GetMatchingHitBoxes(TArray<FGameplayTag> Tags);
	UFUNCTION(BlueprintPure)
	bool GetHitBox(FGameplayTag Tag, FPoCharacterHitBox& Out_HitBox);
};