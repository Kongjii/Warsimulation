// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "AIController.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include <../../../../../../../Source/Runtime/AIModule/Classes/Navigation/PathFollowingComponent.h>
#include "EnemyHPWidget.h"
#include "Components/WidgetComponent.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Components/CapsuleComponent.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h>

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	EnemyHPComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyHPComp"));
	EnemyHPComp->SetupAttachment(RootComponent);

	Base = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	Base->SetupAttachment(RootComponent);

	Center = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Center"));
	Center->SetupAttachment(RootComponent);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	AI = Cast<AAIController>(Controller);

	EnemyHP = Cast<UEnemyHPWidget>(EnemyHPComp->GetWidget());
	EnemyHP->UpdateInfo(1, 1);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto* pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		FVector dir = pc->PlayerCameraManager->GetCameraLocation() - EnemyHPComp->GetComponentLocation();
		EnemyHPComp->SetWorldRotation(UKismetMathLibrary::MakeRotFromX(dir.GetSafeNormal()));
	}

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
	case EEnemyState::Flying:
		TickAttack();
		break;
	case EEnemyState::Die:
		TickAttack();
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
	
}

void AEnemy::TickMove()
{
	
}

void AEnemy::TickAttack()
{
	
}

void AEnemy::TickFlying()
{
	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime > FlyingTime)
	{
		CurrentTime = 0;
		State = EEnemyState::Die;
	}
}

void AEnemy::TickDie()
{
	this->Destroy();
}

void AEnemy::OnMyTakeDamage(int32 damage, UPrimitiveComponent* hitComp)
{
	if (bDie)
		return;

	if (hitComp == Base)
	{
		HP1 -= damage;
		if (HP1 <= 0)
		{
			HP1 = 0;
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, [&]() {
				Destroy();
				}, 1, false);
		}
		EnemyHP->UpdateInfo(HP1, BaseMaxHP);
	}
	else if (hitComp == Center)
	{
		HP2 -= damage;
		if (HP2 <= 0)
		{
			HP2 = 0;
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, [&]() {
				Destroy();
				}, 1, false);
		}
		EnemyHP->UpdateInfo(HP2, CenterMaxHP);
	}

}

void AEnemy::OnMyTakeDamageWithFlying(FVector origin, float upward, FOverlapResult& hitInfo)
{
	State = EEnemyState::Flying;

	AI->StopMovement();

	auto enemy = Cast<AEnemy>(hitInfo.GetActor());
	enemy->GetCharacterMovement()->GravityScale = 1;
	auto comp = enemy->GetCapsuleComponent();

	comp->SetCollisionProfileName("Pawn");

	comp->SetSimulatePhysics(true);

	FVector MyLoc = GetActorLocation();
	FVector force = (MyLoc - origin) + FVector(0, 0, upward);
	force *= FlyingForce;
	comp->AddImpulse(force);
}

