// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHPWidget.h"
#include "Components/ProgressBar.h"

void UEnemyHPWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	float target = BarFront->GetPercent();
	float current = BarBack->GetPercent();
	
	current = FMath::Lerp(current, target, InDeltaTime * 5);
	BarBack->SetPercent(current);
}

void UEnemyHPWidget::UpdateInfo(float cur, float max)
{
	BarFront->SetPercent(cur / max);
}
