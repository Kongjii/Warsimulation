// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8 {
	Search	UMETA(DisplayerName = SEARCH),
	Move	UMETA(DisplayerName = MOVE),
	Attack	UMETA(DisplayerName = ATTACK),
};

UCLASS()
class WARSIMULATION_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	EEnemyState State = EEnemyState::Search;

	UPROPERTY()
	class AAIController* AI;

	UPROPERTY()
	class AActor* Target;

private:
	void TickSearch();
	void TickMove();
	void TickAttack();
};
