// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OpenWorldHUD.generated.h"

class UEcoOverlay;

UCLASS()
class OPENLAND_API AOpenWorldHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditDefaultsOnly, Category = Widget)
	TSubclassOf<UEcoOverlay> EcoOverlayClass;

	UPROPERTY()
	UEcoOverlay* EcoOverlay;

public:
	FORCEINLINE UEcoOverlay* GetEcoOverlay() const { return EcoOverlay; }

};
