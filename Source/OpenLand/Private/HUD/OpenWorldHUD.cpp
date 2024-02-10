// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OpenWorldHUD.h"
#include "HUD/EcoOverlay.h"

void AOpenWorldHUD::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && EcoOverlayClass)
		{
			EcoOverlay = CreateWidget<UEcoOverlay>(Controller, EcoOverlayClass);
			EcoOverlay->AddToViewport();
		}
	}

}
