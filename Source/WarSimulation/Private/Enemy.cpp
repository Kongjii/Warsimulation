// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include <../../../../../../../Source/Runtime/AIModule/Classes/Navigation/PathFollowingComponent.h>
#include <../../../../../../../Source/Runtime/AIModule/Classes/AIController.h>

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	AI = Cast<AAIController>(Controller);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (State)
	{
	case EEnemyState::Search:
		TickSearch();
		break;
	case EEnemyState::Move:
		TickMove();
		break;
	case EEnemyState::Attack:
		TickAttack();
		break;
	default:
		break;
	}

	FString strState = UEnum::GetValueAsString(State);
	DrawDebugString(GetWorld(), GetActorLocation(), strState, nullptr, FColor::White, 0);
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::TickSearch()
{
	TArray<AActor*> towers;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AActor::StaticClass(), TEXT("Tower"), towers);

	if (towers.Num() > 0)
	{
		Target = towers[0];
		State = EEnemyState::Move;
	}
}

void AEnemy::TickMove()
{
	auto result = AI->MoveToLocation(Target->GetActorLocation(), 200);

	if (result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		State = EEnemyState::Attack;
	}
}

void AEnemy::TickAttack()
{
	
}

