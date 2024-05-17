// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunActor.generated.h"

UCLASS()
class WARSIMULATION_API AGunActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGunActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UParticleSystem* FireVFX;

	void OnMyFire();

	bool bGrip;
	void SetGrip(bool _bGrip);
};
