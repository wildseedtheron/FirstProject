#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Core/Abilities/ObkCombatAttributeSet.h"
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
	virtual void PossessedBy(AController* NewController) override;
	//Ability
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<class UOberakAbilitySystemComponent> AbilitySystemComponent;
	//class UAbilitySystemComponent* AbilitySystemComponent;
	//virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void CancelAbilityWithTags(const FGameplayTagContainer CancelWithTags);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Abilities")
	//class UObkCombatAttributeSet* AttributeSet;
	TObjectPtr<UObkCombatAttributeSet> AttributeSet;

	//Health
	void OnHealthChangedInternal(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities|Attributes")
	void OnHealthChanged(float oldValue, float newValue);
	UFUNCTION(BlueprintPure, Category = "Abilities|Attributes")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Abilities|Attributes")
	void SetHealth(float NewVal);
	UFUNCTION(BlueprintPure, Category = "Abilities|Attributes")
	float GetMaxHealth() const;
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

