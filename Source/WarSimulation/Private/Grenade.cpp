// Fill out your copyright notice in the Description page of Project Settings.


#include "Grenade.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "Enemy.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Components/BoxComponent.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Components/StaticMeshComponent.h>

AGrenade::AGrenade()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
}

void AGrenade::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGrenade::Play()
{
	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, [&]() {

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), VFXFactory, GetActorTransform());

		TArray<FOverlapResult> hits;
		FVector origin = GetActorLocation();
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		bool bHit = GetWorld()->OverlapMultiByObjectType(hits, origin, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(1000), params);
		for (int i = 0; i < hits.Num(); i++)
		{
			auto* enemy = Cast<AEnemy>(hits[i].GetActor());
			if (enemy)
			{
				enemy->OnMyTakeDamageWithFlying(origin, 1000, hits[i].GetComponent());
			}
		}
		this->Destroy();

		}, 3, false);
}

