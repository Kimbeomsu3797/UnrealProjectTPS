// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

UCLASS()
class PROJECTTPS_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* springArmComp;
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* tpsCamComp;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputMappingContext* imc_TPS;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_LookUp;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_Turn;

	//좌우 회전 입력 처리
	void Turn(const struct FInputActionValue& inputValue);
	//상하 회전 입력 처리
	void LookUp(const struct FInputActionValue& inputValue);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_Move;
	//이동 속도
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
		float walkSpeed = 600;
	//이동 방향

	FVector direction;

	void Move(const struct FInputActionValue& inputValue);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_Jump;
	//점프 입력 이벤트 처리 함수
	void InputJump(const struct FInputActionValue& inputValue);

	//플레이어 이동 처리
	void PlayerMove();
};
