// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameQuitWidget.generated.h"

/**
 * 
 */
UCLASS()
class WARSIMULATION_API UGameQuitWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, Category = VR, Meta = (BindWidget))
	class UButton* Button_Quit;

	UFUNCTION()
	void OnMyClickGameQuit();
};
