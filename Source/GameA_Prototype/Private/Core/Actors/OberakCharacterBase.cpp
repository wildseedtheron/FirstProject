// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Actors/OberakCharacterBase.h"

// Sets default values
AOberakCharacterBase::AOberakCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOberakCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOberakCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AOberakCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

