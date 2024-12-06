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

	//인풋 매핑 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputMappingContext* imc_TPS;

	//////////////////////////////////////이동 관련 선언 부//////////////////////////////////////
	
	//회전 클래스
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
		float walkSpeed = 200;
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
		float runSpeed = 600;

	//달리기 입력
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_Run;

	//달리기 이벤트 처리 함수
	void InputRun();

	//이동 방향
	FVector direction;
	//이동 조건 함수
	void Move(const struct FInputActionValue& inputValue);
	//실제 이동 함수를 작성
	void PlayerMove();

	//점프 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_Jump;
	//점프 함수 언리얼에서 Jump함수를 지원하기에 함수명을 InputJump로 작성
	void InputJump(const struct FInputActionValue& inputValue);

	//////////////////////////////////////이동 관련 선언 부//////////////////////////////////////

	//////////////////////////////////////무기 관련 선언 부//////////////////////////////////////
	
	//발사 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_Fire;
	//발사 함수
	void InputFire(const struct FInputActionValue& inputValue);

	//각 총 교체 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_GrenadeGun;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_SniperGun;

	//유탄총을 사용하고 있는지에 대한 여부
	bool bUsingGrenadeGun = true;
	//스나이퍼를 사용하고 있는지에 대한 여부
	bool bSniperAim = false;

	//스나이퍼 조준 처리 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_Sniper;
	void SniperAim(const struct FInputActionValue& inputValue);

	//스나이퍼 조준 UI 위젯 공장
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
		TSubclassOf<class UUserWidget> sniperUIFactory;
	//스나이퍼 UI 위젯 인스턴스
	UPROPERTY()
		class UUserWidget* _sniperUI;
	//일반 조준 크로스헤어UI 위젯 공장
	UPROPERTY(EditDefaultsOnly, Category = BulletEffect)
		TSubclassOf<class UUserWidget> crosshairUIFactory;
	//크로스헤어 UI 위젯 인스턴스
	UPROPERTY()
	class UUserWidget* _crosshairUI;

	//카메라 셰이크 블루프린트를 저장할 변수
	UPROPERTY(EditDefaultsOnly, Category = CameraMotion)
		TSubclassOf<class UCameraShakeBase> cameraShake;
	//총알 발사 사운드
	UPROPERTY(EditDefaultsOnly, Category = Sound)
		class USoundBase* bulletSound;

	//각 무기 교체 함수
	void ChangeToGrenadeGun(const struct FInputActionValue& inputValue);

	void ChangeToSniperGun(const struct FInputActionValue& inputValue);

	//총알 적용 공장 (프리팹생성 느낌)
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
		TSubclassOf<class ABullet> bulletFactory;

	//이펙트 효과 적용 공장
	UPROPERTY(EditAnywhere, Category = BulletEffect)
		class UParticleSystem* bulletEffectFactory;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	//스프링암 클래스
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* springArmComp;
	//카메라 클래스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* tpsCamComp;
	//총 스켈레탈메시 클래스
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
		class USkeletalMeshComponent* gunMeshComp;
	//스나이프 스테틱 메시 클래스
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
		class UStaticMeshComponent* sniperGunComp;
};
