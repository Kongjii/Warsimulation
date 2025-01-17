// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <../../../../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h>
#include "VRPlayer.generated.h"

struct FInputActionValue;

UCLASS()
class WARSIMULATION_API AVRPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	AVRPlayer();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* VRCamera;

	UPROPERTY(EditDefaultsOnly)
	class UMotionControllerComponent* MotionLeft;

	UPROPERTY(EditDefaultsOnly)
	class UMotionControllerComponent* MotionRight;

	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* MeshLeft;

	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* MeshRight;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputMappingContext* IMC_VRPlayer;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_Move;

	void OnIAMove(const FInputActionValue& value);

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_Turn;

	void OnIATurn(const FInputActionValue& value);

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UNiagaraComponent* TeleportCircleVFX;

	bool bTeleporting;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_Teleport;

	void ONIATeleportStart(const FInputActionValue& value);
	void ONIATeleportEnd(const FInputActionValue& value);

	void DrawLine(const FVector& start, const FVector& end);

	bool HitTest(FVector start, FVector end, FHitResult& OuthitInfo);
	void ResetTeleport();

	FVector TeleportLocation;
	void DoTeleport();

	UPROPERTY(EditDefaultsOnly, Category = VR)
	bool bTeleportCurve = true;

	void TickLine();
	void TickCurve();

	bool CheckHitTeleport(const FVector& start, FVector& end);

	UPROPERTY(EditDefaultsOnly, Category = VR)
	int32 CurveStep = 200;

	TArray<FVector> Points;
	void MakeCurvePoints();

	void DrawCurve(int max);

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UNiagaraComponent* TeleportTraceVFX;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	bool bWarp = true;

	FTimerHandle WarpTimerHandle;
	float CurrentTime;
	float WarpTime = 0.2f;

	void DoWarp();

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UMotionControllerComponent* RightAim;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_Fire;

	void OnIAFire(const FInputActionValue& value);
	UPROPERTY(EditDefaultsOnly, Category = VR)

	class UParticleSystem* FireVFX;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	TSubclassOf<AActor> CrosshairFactory;

	UPROPERTY()
	AActor* Crosshair;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	float kAdjustCrosshairScale = 0.1f;

	void DrawCrosshair();

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_ViewReset;
	void OnIAViewReset(const FInputActionValue& value);

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_RemoteGrip;

	void OnIARemoteGrip(const FInputActionValue& value);
	void OnIARemoteUnGrip(const FInputActionValue& value);

	FTimerHandle RemoteGripTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UHapticFeedbackEffect_Curve* HapticFire;

	// 오른손
	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_Grip;

	void OnIAGrip(const FInputActionValue& value);
	void OnIAUnGrip(const FInputActionValue& value);

	UPROPERTY()
	class UPrimitiveComponent* GripObject;

	FVector ThrowDirection;
	FVector PrevLocation;
	FQuat PrevRotation;
	FQuat deltaAngle;

	// 왼손
	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UPrimitiveComponent* GripObjectLeft;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_GripLeft;

	void OnIAGripLeft(const FInputActionValue& value);
	void OnIAUnGripLeft(const FInputActionValue& value);

	FVector ThrowDirectionLeft;
	FVector PrevLocationLeft;
	FQuat PrevRotationLeft;
	FQuat deltaAngleLeft;

	UPROPERTY()
	class AGunActor* GunActor;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_LeftFire;

	void OnIALeftFire(const FInputActionValue& value);

	// 양손
	struct FOverlapResult DoGrip(class USkeletalMeshComponent* hand);
	void DoUnGrip(class USkeletalMeshComponent* hand, UPrimitiveComponent* obj, const FQuat _deltaAngle);

	UPROPERTY(EditDefaultsOnly, Category = VR)
	float GripRadius = 100;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	float ThrowPower = 50000;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	float torquePower = 500;

	void TickGripCalc();
	void DoThrowObject(class UPrimitiveComponent* obj, const FQuat& _deltaAngle);

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UWidgetInteractionComponent* Interation;

	void OnIAReleasePointer(const FInputActionValue& value);
};
