// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EcoOverlay.generated.h"

/**
 * 
 */
UCLASS()
class OPENLAND_API UEcoOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGold(int32 Gold);
	void SetSouls(int32 Souls);

private:

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CoinText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SoulsText;
	
};
