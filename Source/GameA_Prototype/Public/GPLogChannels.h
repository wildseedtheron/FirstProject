// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

GAMEA_PROTOTYPE_API DECLARE_LOG_CATEGORY_EXTERN(LogLyra, Log, All);
GAMEA_PROTOTYPE_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraExperience, Log, All);
GAMEA_PROTOTYPE_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraAbilitySystem, Log, All);
GAMEA_PROTOTYPE_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraTeams, Log, All);

GAMEA_PROTOTYPE_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
