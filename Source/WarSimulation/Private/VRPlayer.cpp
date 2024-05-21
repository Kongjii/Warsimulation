// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPlayer.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Components/SkeletalMeshComponent.h>
#include <../../../../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h>
#include <../../../../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h>
#include <Components/CapsuleComponent.h>
#include <../../../../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include <../../../../../../../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraDataInterfaceArrayFunctionLibrary.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>
#include <../../../../../../../Plugins/Runtime/XRBase/Source/XRBase/Public/HeadMountedDisplayFunctionLibrary.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Haptics/HapticFeedbackEffect_Curve.h>
#include "GunActor.h"
#include "Enemy.h"
#include "Grenade.h"
#include <../../../../../../../Source/Runtime/UMG/Public/Components/WidgetInteractionComponent.h>

AVRPlayer::AVRPlayer()
{
	PrimaryActorTick.bCanEverTick = true;	

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(RootComponent);

	MotionLeft = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionLeft"));
	MotionLeft->SetTrackingMotionSource(TEXT("Left"));
	MotionLeft->SetupAttachment(RootComponent);

	MotionRight = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionRight"));
	MotionRight->SetTrackingMotionSource(TEXT("Right"));
	MotionRight->SetupAttachment(RootComponent);

	MeshLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshLeft"));
	MeshLeft->SetupAttachment(MotionLeft);

	MeshRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshRight"));
	MeshRight->SetupAttachment(MotionRight);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMeshLeft(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_MannyXR_left.SKM_MannyXR_left'"));
	if (TempMeshLeft.Succeeded())
	{
		MeshLeft->SetSkeletalMesh(TempMeshLeft.Object);
		MeshLeft->SetWorldLocationAndRotation(FVector(-2.981260f, -3.5f, 4.561753f), FRotator(-25, -180, 90));
	}

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMeshRight(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_MannyXR_right.SKM_MannyXR_right'"));
	if (TempMeshRight.Succeeded())
	{
		MeshRight->SetSkeletalMesh(TempMeshRight.Object);
		MeshRight->SetWorldLocationAndRotation(FVector(-2.981260f, 3.5f, 4.561753f), FRotator(25, 0, 90));
	}

	TeleportCircleVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleportCircleVFX"));
	TeleportCircleVFX->SetupAttachment(RootComponent);
	TeleportCircleVFX->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TeleportTraceVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleportTraceVFX"));
	TeleportCircleVFX->SetupAttachment(RootComponent);

	RightAim = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightAim"));
	RightAim->SetupAttachment(RootComponent);
	RightAim->SetTrackingMotionSource(TEXT("RightAim"));

	Interation = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("Interaction"));
	Interation->SetupAttachment(RightAim);
}

void AVRPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	auto* pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsystem)
		{
			subsystem->AddMappingContext(IMC_VRPlayer, 0);
		}
	}
	ResetTeleport();

	Crosshair = GetWorld()->SpawnActor<AActor>(CrosshairFactory);
}

void AVRPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawCrosshair();

	TickGripCalc();

	if (GripObject)
		GripObject->SetRelativeLocationAndRotation(FVector(3.2f, 25.0f, -2.0f), FRotator(0.0f, 90.0f, 270.0f));

	if (true == bTeleporting)
	{
		if (bTeleportCurve)
		{
			UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(TeleportTraceVFX, FName("User.PointArray"), Points);
			if (bTeleportCurve)
			{
				TickCurve();
			}
			else
			{
				TickLine();
			}
		}
	}
}

void AVRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (input)
	{
		input->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AVRPlayer::OnIAMove);
		input->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &AVRPlayer::OnIATurn);

		input->BindAction(IA_Teleport, ETriggerEvent::Started, this, &AVRPlayer::ONIATeleportStart);
		input->BindAction(IA_Teleport, ETriggerEvent::Completed, this, &AVRPlayer::ONIATeleportEnd);

		input->BindAction(IA_Fire, ETriggerEvent::Started, this, &AVRPlayer::OnIAFire);
		input->BindAction(IA_Fire, ETriggerEvent::Completed, this, &AVRPlayer::OnIAReleasePointer);

		input->BindAction(IA_LeftFire, ETriggerEvent::Started, this, &AVRPlayer::OnIALeftFire);

		input->BindAction(IA_Grip, ETriggerEvent::Started, this, &AVRPlayer::OnIAGrip);
		input->BindAction(IA_Grip, ETriggerEvent::Completed, this, &AVRPlayer::OnIAUnGrip);

		input->BindAction(IA_GripLeft, ETriggerEvent::Started, this, &AVRPlayer::OnIAGripLeft);
		input->BindAction(IA_GripLeft, ETriggerEvent::Completed, this, &AVRPlayer::OnIAUnGripLeft);

		input->BindAction(IA_ViewReset, ETriggerEvent::Started, this, &AVRPlayer::OnIAViewReset);

		input->BindAction(IA_RemoteGrip, ETriggerEvent::Started, this, &AVRPlayer::OnIARemoteGrip);
		input->BindAction(IA_RemoteGrip, ETriggerEvent::Completed, this, &AVRPlayer::OnIARemoteUnGrip);
	}
}

void AVRPlayer::OnIAMove(const FInputActionValue& value)
{
	FVector2D v = value.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), v.Y);
	AddMovementInput(GetActorRightVector(), v.X);
}

void AVRPlayer::OnIATurn(const FInputActionValue& value)
{
	float v = value.Get<float>();
	AddControllerYawInput(v);
}

void AVRPlayer::ONIATeleportStart(const FInputActionValue& value)
{
	bTeleporting = true;
	TeleportTraceVFX->SetVisibility(true);
}

void AVRPlayer::ONIATeleportEnd(const FInputActionValue& value)
{
	if (TeleportCircleVFX->GetVisibleFlag()) // 활성화 여부
	{
		if (bWarp)
		{
			DoWarp();
		}
		else
		{
			DoTeleport();
		}
	}
	ResetTeleport();
}

void AVRPlayer::DrawLine(const FVector& start, const FVector& end)
{
	//DrawDebugLine(GetWorld(), start, end, FColor::Red, false, -1, 0, 1);
}

bool AVRPlayer::HitTest(FVector start, FVector end, FHitResult& OuthitInfo)
{
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.AddIgnoredComponent(MeshLeft);
	params.AddIgnoredComponent(MeshRight);

	return GetWorld()->LineTraceSingleByChannel(OuthitInfo, start, end, ECC_Visibility, params);
}

void AVRPlayer::ResetTeleport()
{
	TeleportCircleVFX->SetVisibility(false);
	bTeleporting = false;
	TeleportTraceVFX->SetVisibility(false);
}

void AVRPlayer::DoTeleport()
{
	FVector height = FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	SetActorLocation(TeleportLocation + height);
}

void AVRPlayer::TickLine()
{
	FVector start = MeshRight->GetComponentLocation();
	FVector end = start + MeshRight->GetRightVector() * 100000;

	CheckHitTeleport(start, end);

	DrawLine(start, end);

	Points.Empty(2);
	Points.Add(start);
	Points.Add(end);
}

void AVRPlayer::TickCurve()
{
	MakeCurvePoints();

	FHitResult hitInfo;
	int maxPoints = Points.Num();
	for (int i = 0; i < Points.Num() - 1; i++)
	{
		if (CheckHitTeleport(Points[i], Points[i + 1]))
		{
			maxPoints = i + 1;
			break;
		}
	}

	Points.SetNum(maxPoints);

	DrawCurve(maxPoints);
}

bool AVRPlayer::CheckHitTeleport(const FVector& start, FVector& end)
{
	FHitResult hitInfo;
	bool bHit = HitTest(start, end, hitInfo);
	if (bHit && hitInfo.GetActor()->GetActorNameOrLabel().Contains(TEXT("Floor")))
	{
		end = hitInfo.ImpactPoint;
		TeleportLocation = hitInfo.Location; 
		TeleportCircleVFX->SetWorldLocation(hitInfo.Location);
		TeleportCircleVFX->SetVisibility(true);
	}
	else
	{
		TeleportCircleVFX->SetVisibility(false);
	}
	return bHit;
}

void AVRPlayer::MakeCurvePoints()
{
	Points.Empty(CurveStep);
	FVector gravity(0, 0, -981);
	float simDT = 1.f / 60.f;
	FVector Point = MeshRight->GetComponentLocation();
	FVector velocity = MeshRight->GetRightVector() * 1000;
	Points.Add(Point);

	for (int i = 0; i < CurveStep; i++)
	{
		Point += velocity * simDT + 0.5f * gravity * simDT * simDT;
		velocity += gravity * simDT;
		Points.Add(Point);
	}
}

void AVRPlayer::DrawCurve(int max)
{
	for (int i = 0; i < max - 1; i++)
	{
		DrawLine(Points[i], Points[i + 1]);
	}
}

void AVRPlayer::DoWarp()
{
	if ( false == bWarp )
		return;

	CurrentTime = 0;
	
	FVector height = FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	FVector tarLoc = TeleportLocation + height;
	FVector originLoc = GetActorLocation();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetWorld()->GetTimerManager().SetTimer(WarpTimerHandle, [&, originLoc, tarLoc]() {
			CurrentTime += GetWorld()->GetDeltaSeconds();

			float alpha = CurrentTime / WarpTime;
			FVector curLoc = FMath::Lerp(originLoc, tarLoc, alpha);
			SetActorLocation(curLoc);

			if (alpha >= 1) {
				GetWorld()->GetTimerManager().ClearTimer(WarpTimerHandle);
				SetActorLocation(tarLoc);
				GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			}
	}, 0.033333f, true);
}

void AVRPlayer::OnIAFire(const FInputActionValue& value)
{
	if (Interation)
	{
		Interation->PressPointerKey(EKeys::LeftMouseButton);
	}

	auto* pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		pc->PlayHapticEffect(HapticFire, EControllerHand::Right);
	}

	FHitResult hitInfo;
	FVector start = RightAim->GetComponentLocation();
	FVector end = start + RightAim->GetForwardVector() * 100000;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.AddIgnoredComponent(MeshRight);

	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params);
	if ( bHit )
	{
		auto* hitComp = hitInfo.GetComponent();
// 		if ( hitComp )
// 		{
// 			hitComp->GetName().Contains(TEXT("center"));
// 			UE_LOG(LogTemp, Warning, TEXT("true"));
// 		}
		if (hitComp && hitComp->IsSimulatingPhysics())
		{
			FVector direction = (end - start).GetSafeNormal();
			FVector force = direction * 1000 * hitComp->GetMass();
			hitComp->AddImpulseAtLocation(force, hitInfo.ImpactPoint);
		}

		auto* enemy = Cast<AEnemy>(hitInfo.GetActor());
		if (enemy)
		{
			enemy->OnMyTakeDamage(1, hitComp);
		}

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireVFX, hitInfo.ImpactPoint);
	}
	else
	{
		// 허공
	}
}

void AVRPlayer::DrawCrosshair()
{
	FVector start = RightAim->GetComponentLocation();
	FVector end = start + RightAim->GetForwardVector() * 100000;
	FHitResult hitInfo;
	bool bHit = HitTest(start, end, hitInfo);
	float distance = 1;
	if (bHit)
	{
		Crosshair->SetActorLocation(hitInfo.ImpactPoint);
		DrawDebugLine(GetWorld(), start, hitInfo.ImpactPoint, FColor::Red, false, 0);
		distance = kAdjustCrosshairScale * hitInfo.Distance / 100;
	}
	else
	{
		Crosshair->SetActorLocation(end);
		DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 0);
		distance = kAdjustCrosshairScale * FVector::Dist(start, end) / 100;
	}

	Crosshair->SetActorScale3D(FVector(distance));

	FVector dir = Crosshair->GetActorLocation() - VRCamera->GetComponentLocation();
	Crosshair->SetActorRotation(UKismetMathLibrary::MakeRotFromX(dir.GetSafeNormal()));
}

void AVRPlayer::OnIAGrip(const FInputActionValue& value)
{
	FOverlapResult hitObject = DoGrip(MeshRight);

// 		hits.Sort([&](const FOverlapResult a, const FOverlapResult b) {
// 			float distA = FVector::Dist(origin, a.GetComponent()->GetComponentLocation());
// 			float distB = FVector::Dist(origin, b.GetComponent()->GetComponentLocation());
// 			return distA < distB;
// 		});
	if (nullptr != hitObject.GetActor())
	{
		GripObject = hitObject.GetComponent();

		GripObject->SetSimulatePhysics(false);
		GripObject->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GripObject->AttachToComponent(MeshRight, FAttachmentTransformRules::KeepWorldTransform);

		GripObject->IgnoreComponentWhenMoving(GetCapsuleComponent(), true);
	}
}

void AVRPlayer::OnIAUnGrip(const FInputActionValue& value)
{
	if ( nullptr == GripObject )
		return;

	DoUnGrip(MeshRight, GripObject, deltaAngle);

	GripObject = nullptr;
}

void AVRPlayer::TickGripCalc()
{
	// 오른손
	ThrowDirection = MeshRight->GetComponentLocation() - PrevLocation;

	deltaAngle = MeshRight->GetComponentQuat() * PrevRotation.Inverse();

	PrevLocation = MeshRight->GetComponentLocation();
	PrevRotation = MeshRight->GetComponentQuat();

	// ======================
	// 왼손
	ThrowDirectionLeft = MeshLeft->GetComponentLocation() - PrevLocationLeft;

	deltaAngleLeft = MeshLeft->GetComponentQuat() * PrevRotationLeft.Inverse();

	PrevLocationLeft = MeshLeft->GetComponentLocation();
	PrevRotationLeft = MeshLeft->GetComponentQuat();
}

void AVRPlayer::DoThrowObject(class UPrimitiveComponent* obj, const FQuat& _deltaAngle)
{
	if ( obj )
	{
		obj->AddForce(ThrowDirection.GetSafeNormal() * obj->GetMass() * ThrowPower);

		FVector axis;
		float angle;
		_deltaAngle.ToAxisAndAngle(axis, angle);
		float dt = GetWorld()->GetDeltaSeconds();

		// 각속도 : angle(radian) / dt * axis
		FVector angularVelocity = angle / dt * axis;
		obj->SetPhysicsAngularVelocityInRadians(angularVelocity, true);

		auto* grenade = Cast<AGrenade>(obj->GetOwner());
		if (grenade)
		{
			// 수류탄의 Play를 호출하고 싶다.
			grenade->Play();
		}
	}
}

void AVRPlayer::OnIAReleasePointer(const FInputActionValue& value)
{
	if (Interation)
	{
		Interation->ReleasePointerKey(EKeys::LeftMouseButton);
	}
}

void AVRPlayer::OnIAViewReset(const FInputActionValue& value)
{
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		auto* pc = Cast<APlayerController>(Controller);
		auto conRot = pc->GetControlRotation();
		conRot.Yaw = 0;
		pc->SetControlRotation(conRot);
		UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(conRot.Yaw);
	}
}

void AVRPlayer::OnIARemoteGrip(const FInputActionValue& value)
{
	if ( GripObject )
		return;

	FHitResult hitInfo;
	FVector start = MeshRight->GetComponentLocation();
	FVector end = start + MeshRight->GetRightVector() * 100000;
	
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.AddIgnoredComponent(MeshRight);
	params.AddIgnoredComponent(MeshLeft);

	bool bHit = GetWorld()->SweepSingleByObjectType(hitInfo, start, end, FQuat::Identity, ECC_PhysicsBody, FCollisionShape::MakeSphere(20), params);
	if (bHit && hitInfo.GetComponent()->IsSimulatingPhysics())
	{
		GripObject = hitInfo.GetComponent();

		GripObject->SetSimulatePhysics(false);
		GripObject->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GripObject->AttachToComponent(MeshRight, FAttachmentTransformRules::KeepWorldTransform);

		GripObject->IgnoreComponentWhenMoving(GetCapsuleComponent(), true);

		GripObject->SetRelativeLocationAndRotation(FVector(70.0f, 0.0f, 30.0f), FRotator(0.0f, 90.0f, 270.0f));
	}

	float dt = 1.f / 60.f;

// 	if (GripObject)
// 	{
// 		GetWorldTimerManager().SetTimer(RemoteGripTimerHandle, [&, dt]() {
// 			FVector p = GripObject->GetComponentLocation();
// 			FVector target = MeshRight->GetComponentLocation();
// 			p = FMath::Lerp(p, target, GetWorld()->GetDeltaSeconds() * 6);
// 			GripObject->SetWorldLocation(p);
// 
// 			if (FVector::Dist(p, target) < 10)
// 			{
// 				GripObject->SetWorldLocation(target);
// 				GetWorldTimerManager().ClearTimer(RemoteGripTimerHandle);
// 			}
// 		}, dt, true);
// 	}
}

void AVRPlayer::OnIARemoteUnGrip(const FInputActionValue& value)
{
	GetWorldTimerManager().ClearTimer(RemoteGripTimerHandle);
	OnIAUnGrip(value);
}

void AVRPlayer::OnIAGripLeft(const FInputActionValue& value)
{
	FOverlapResult hitObject = DoGrip(MeshLeft);

	if (nullptr != hitObject.GetActor())
	{
		if (hitObject.GetActor()->Tags.Contains(TEXT("Gun")) )
		{
			GunActor = Cast<AGunActor>(hitObject.GetActor());
			GunActor->SetGrip(true);
		}

		GripObjectLeft = hitObject.GetComponent();

		GripObjectLeft->SetSimulatePhysics(false);
		GripObjectLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GripObjectLeft->IgnoreComponentWhenMoving(GetCapsuleComponent(), true);

		GripObjectLeft->AttachToComponent(MeshLeft, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void AVRPlayer::OnIAUnGripLeft(const FInputActionValue& value)
{
	if ( nullptr == GripObjectLeft )
		return;

	if (GunActor)
	{
		GunActor->SetGrip(false);
		GunActor = nullptr;
	}

	DoUnGrip(MeshLeft, GripObjectLeft, deltaAngleLeft);

	GripObjectLeft = nullptr;
}

void AVRPlayer::OnIALeftFire(const FInputActionValue& value)
{
	if (GunActor)
	{
		GunActor->OnMyFire();
	}
}

struct FOverlapResult AVRPlayer::DoGrip(class USkeletalMeshComponent* hand)
{
	TArray<FOverlapResult> hits;
	FVector origin = hand->GetComponentLocation();
	FQuat rot = FQuat::Identity;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.AddIgnoredComponent(MeshLeft);
	params.AddIgnoredComponent(MeshRight);

	bool bHits = GetWorld()->OverlapMultiByObjectType(hits, origin, rot, ECC_PhysicsBody, FCollisionShape::MakeSphere(GripRadius), params);

	if ( bHits )
	{
		int32 index = -1;
		float dist = 999999999;
		for (int i = 0; i < hits.Num(); i++)
		{
			auto* temp = hits[i].GetComponent();
			float tempDist = FVector::Dist(origin, temp->GetComponentLocation());
			if (dist > tempDist)
			{
				dist = tempDist;
				index = i;
			}
		}
		return hits[index];
	}
	return FOverlapResult();
}

void AVRPlayer::DoUnGrip(class USkeletalMeshComponent* hand, UPrimitiveComponent* obj, const FQuat _deltaAngle)
{
	obj->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	obj->SetSimulatePhysics(true);
	obj->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	DoThrowObject(obj, _deltaAngle);
}