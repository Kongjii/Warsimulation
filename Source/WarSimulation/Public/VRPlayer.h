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
	// Sets default values for this character's properties
	AVRPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// VR카메라 컴포넌트를 생성하고 루트에 붙이고 싶다.
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* VRCamera;

	// 모션컨트롤러 왼손, 오른손 생성하고 루트에 붙이고 싶다.
	UPROPERTY(EditDefaultsOnly)
	class UMotionControllerComponent* MotionLeft;

	UPROPERTY(EditDefaultsOnly)
	class UMotionControllerComponent* MotionRight;

	// 왼손, 오른손 스켈레탈메시컴포넌트를 만들고 모션 컨트롤러에 붙이고 싶다.
	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* MeshLeft;

	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* MeshRight;

	// 왼손, 오른손 스켈레탈메시를 로드해서 적용하고 싶다.
	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputMappingContext* IMC_VRPlayer;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_Move;

	// Move 생성
	void OnIAMove(const FInputActionValue& value);

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_Turn;

	// 마우스 좌우 생성
	void OnIATurn(const FInputActionValue& value);

	// 텔레포트 처리를 위해서 써클을 표현하고 싶다.
	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UStaticMeshComponent* TeleportCircle;

	// 텔레포트중인가? 여부를 기억하고 싶다.
	bool bTeleporting;

	// 입력처리와 그에 해당하는 함수를 구현하고 싶다.
	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_Teleport;

	// Teleport 생성
	void ONIATeleportStart(const FInputActionValue& value);
	void ONIATeleportEnd(const FInputActionValue& value);

	void DrawLine(const FVector& start, const FVector& end);
private:
	bool HitTest(FVector start, FVector end, FHitResult& OuthitInfo);
	void ResetTeleport();
};
