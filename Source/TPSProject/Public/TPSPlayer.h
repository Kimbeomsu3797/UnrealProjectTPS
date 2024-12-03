// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

UCLASS()
class TPSPROJECT_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

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

	//이동 입력 처리
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_Move;
	//이동 속도 처리
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
		float walkSpeed = 600;
	//이동 방향
	FVector direction;
	//이동 함수 조건 함수
	void Move(const struct FInputActionValue& inputValue);
	//실제 이동 함수를 작성
	void PlayerMove();
	//점프 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_Jump;
	//점프 함수 언리얼에서 Jump함수를 지원하기에 함수명을 InputJump로 작성
	void InputJump(const struct FInputActionValue& inputValue);

	////발사 클래스
	//UPROPERTY(EditDefaultsOnly, Category = "Input")
	//	class UInputAction* ia_Fire;
	////발사 함수
	//void Fire(const struct FInputActionValue& inputValue);



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
	//총 스켈레탈메시 클래스
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
		class USkeletalMeshComponent* gunMeshComp;
};
