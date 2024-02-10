// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EcoOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UEcoOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UEcoOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void UEcoOverlay::SetGold(int32 Gold)
{
	if (CoinText)
	{
		const FString String = FString::Printf(TEXT("%d"), Gold);
		const FText Text = FText::FromString(String);
		CoinText->SetText(Text);
	}
}

void UEcoOverlay::SetSouls(int32 Souls)
{
	if (SoulsText)
	{
		const FString String = FString::Printf(TEXT("%d"), Souls);
		const FText Text = FText::FromString(String);
		SoulsText->SetText(Text);
	}
}
