// Fill out your copyright notice in the Description page of Project Settings.


#include "GunActor.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>

AGunActor::AGunActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AGunActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGunActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bGrip)
	{
		FHitResult hitInfo;
		FVector start = GetActorLocation() + GetActorForwardVector() * 50;
		FVector end = start + GetActorForwardVector() * 100000;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params);
		if (bHit)
		{
			DrawDebugLine(GetWorld(), start, hitInfo.ImpactPoint, FColor::Red, false, 0);
		}
		else
		{
			DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 0);
		}
	}
}

void AGunActor::OnMyFire()
{
	FHitResult hitInfo;
	FVector start = GetActorLocation() + GetActorForwardVector() * 50;
	FVector end = start + GetActorForwardVector() * 100000;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params);
	if (bHit)
	{
		
		auto* hitComp = hitInfo.GetComponent();
		if (hitComp && hitComp->IsSimulatingPhysics())
		{
			FVector direction = (end - start).GetSafeNormal();
			FVector force = direction * 1000 * hitComp->GetMass(); 
			hitComp->AddImpulseAtLocation(force, hitInfo.ImpactPoint);
		}

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireVFX, hitInfo.ImpactPoint);
	}
}

void AGunActor::SetGrip(bool _bGrip)
{
	bGrip = _bGrip;
}

