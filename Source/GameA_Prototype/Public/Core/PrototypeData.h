#pragma once

#include "PrototypeData.generated.h"

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None,
	Confirm,
	Cancel,
	Punch,
	Kick,
	Fire,
	Jump
};