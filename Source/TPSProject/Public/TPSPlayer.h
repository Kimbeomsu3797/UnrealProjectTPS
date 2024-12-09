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

	//��ǲ ���� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputMappingContext* imc_TPS;

	//////////////////////////////////////�̵� ���� ���� ��//////////////////////////////////////
	
	




	//�޸��� �Է�
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_Run;

	//�޸��� �̺�Ʈ ó�� �Լ�
	void InputRun();
	
	
	

	//���� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_Jump;
	//���� �Լ� �𸮾󿡼� Jump�Լ��� �����ϱ⿡ �Լ����� InputJump�� �ۼ�
	void InputJump(const struct FInputActionValue& inputValue);

	//////////////////////////////////////�̵� ���� ���� ��//////////////////////////////////////

	//////////////////////////////////////���� ���� ���� ��//////////////////////////////////////
	
	//�߻� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_Fire;
	//�߻� �Լ�
	void InputFire(const struct FInputActionValue& inputValue);

	//�� �� ��ü Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_GrenadeGun;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_SniperGun;

	//��ź���� ����ϰ� �ִ����� ���� ����
	bool bUsingGrenadeGun = true;
	//�������۸� ����ϰ� �ִ����� ���� ����
	bool bSniperAim = false;

	//�������� ���� ó�� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Input")
		class UInputAction* ia_Sniper;
	void SniperAim(const struct FInputActionValue& inputValue);

	//�������� ���� UI ���� ����
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
		TSubclassOf<class UUserWidget> sniperUIFactory;
	//�������� UI ���� �ν��Ͻ�
	UPROPERTY()
		class UUserWidget* _sniperUI;
	//�Ϲ� ���� ũ�ν����UI ���� ����
	UPROPERTY(EditDefaultsOnly, Category = BulletEffect)
		TSubclassOf<class UUserWidget> crosshairUIFactory;
	//ũ�ν���� UI ���� �ν��Ͻ�
	UPROPERTY()
	class UUserWidget* _crosshairUI;

	//ī�޶� ����ũ �������Ʈ�� ������ ����
	UPROPERTY(EditDefaultsOnly, Category = CameraMotion)
		TSubclassOf<class UCameraShakeBase> cameraShake;
	//�Ѿ� �߻� ����
	UPROPERTY(EditDefaultsOnly, Category = Sound)
		class USoundBase* bulletSound;

	//�� ���� ��ü �Լ�
	void ChangeToGrenadeGun(const struct FInputActionValue& inputValue);

	void ChangeToSniperGun(const struct FInputActionValue& inputValue);

	//�Ѿ� ���� ���� (�����ջ��� ����)
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
		TSubclassOf<class ABullet> bulletFactory;

	//����Ʈ ȿ�� ���� ����
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
	//�������� Ŭ����
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* springArmComp;
	//ī�޶� Ŭ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* tpsCamComp;
	//�� ���̷�Ż�޽� Ŭ����
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
		class USkeletalMeshComponent* gunMeshComp;
	//�������� ����ƽ �޽� Ŭ����
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
		class UStaticMeshComponent* sniperGunComp;
public:
	UPROPERTY(VisibleAnywhere, Category= Component)
	class UPlayerBaseComponent* playerMove;
};
