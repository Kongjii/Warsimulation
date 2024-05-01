// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <../../../../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h>
#include "VRPlayer.generated.h"

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

	// VRī�޶� ������Ʈ�� �����ϰ� ��Ʈ�� ���̰� �ʹ�.
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* VRCamera;

	// �����Ʈ�ѷ� �޼�, ������ �����ϰ� ��Ʈ�� ���̰� �ʹ�.
	UPROPERTY(EditDefaultsOnly)
	class UMotionControllerComponent* MotionLeft;

	UPROPERTY(EditDefaultsOnly)
	class UMotionControllerComponent* MotionRight;

	// �޼�, ������ ���̷�Ż�޽�������Ʈ�� ����� ��� ��Ʈ�ѷ��� ���̰� �ʹ�.
	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* MeshLeft;

	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* MeshRight;

	// �޼�, ������ ���̷�Ż�޽ø� �ε��ؼ� �����ϰ� �ʹ�.
	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputMappingContext* IMC_VRPlayer;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_Move;

	// Move ����
	void OnIAMove(const FInputActionValue& value);
};