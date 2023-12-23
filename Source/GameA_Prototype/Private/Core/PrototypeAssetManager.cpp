// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PrototypeAssetManager.h"
#include "Core/Tags/PrototypeGameplayTags.h"
 
UPrototypeAssetManager::UPrototypeAssetManager() {}

UPrototypeAssetManager& UPrototypeAssetManager::Get()
{
	check(GEngine);

	UPrototypeAssetManager* PrototypeAssetManager = Cast<UPrototypeAssetManager>(GEngine->AssetManager);
	return *PrototypeAssetManager;
}

void UPrototypeAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	//Load Native Tags
	FPrototypeGameplayTags::InitializeNativeTags();
}