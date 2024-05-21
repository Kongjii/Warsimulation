// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHPWidget.generated.h"

/**
 * 
 */
UCLASS()
class WARSIMULATION_API UEnemyHPWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void UpdateInfo(float cur, float max);

	UPROPERTY(EditDefaultsOnly, Category = VR, meta=(BindWidget))
	class UProgressBar* BarBack;

	UPROPERTY(EditDefaultsOnly, Category = VR, meta=(BindWidget))
	class UProgressBar* BarFront;
};
