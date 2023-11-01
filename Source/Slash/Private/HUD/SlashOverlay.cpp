// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlay::SetHealthBarPercent(float percent)
{
	if(HealthProgressBar)
	{
		HealthProgressBar->SetPercent(percent);
	}
}

void USlashOverlay::SetStaminaBarPercent(float percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(percent);
	}
}

void USlashOverlay::SetCoinCount(int32 count)
{
	if (CoinCountText)
	{
		CoinCountText->SetText(IntToFText(count));
	}
}

FText USlashOverlay::IntToFText(const int32& count)
{
	return FText::FromString(FString::Printf(TEXT("%d"), count));
}

void USlashOverlay::SetSoulsCount(int32 count)
{
	if (SoulsCountText)
	{
		SoulsCountText->SetText(IntToFText(count));
	}
}
