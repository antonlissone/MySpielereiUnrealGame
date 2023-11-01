// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaProgressBar;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CoinCountText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SoulsCountText;

public:

	void SetHealthBarPercent(float percent);
	void SetStaminaBarPercent(float percent);
	FText IntToFText(const int32& count);
	void SetCoinCount(int32 count);
	void SetSoulsCount(int32 count);

};
