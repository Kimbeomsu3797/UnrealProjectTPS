// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Bullet.h"
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>

#include "EnemyFSM.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//1.���̷�Ż�޽� �����͸� �ҷ�����ʹ�.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh
	(TEXT("SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		//2. Mesh ������Ʈ�� ��ġ�� �����ϰ� �ʹ�.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	//3.TPS ī�޶� ���̰� �ʹ�.
	//3-1. SpringArm������Ʈ ���̱�
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringAtmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));
	springArmComp->TargetArmLength = 400;
	//�ش� ��Ʈ�ѷ� �����̼��� Ʈ���
	springArmComp->bUsePawnControlRotation = true;
	//3-2. Camera������Ʈ�� ���δ�.

	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);
	//�ش� ��Ʈ�ѷ��� Yaw�� Ʈ��� ���� Yaw�� z�� ȸ�� Roll�� X�� ȸ�� Pich�� Y�� ȸ��
	tpsCamComp->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = true;
	JumpMaxCount = 2;

	// 4. �� ���̷�Ż�޽� ������Ʈ ���
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	// 4-1. �θ� ������Ʈ�� Mesh ������Ʈ�� ����
	gunMeshComp->SetupAttachment(GetMesh());
	// 4-2. ���̷�Ż�޽� ������ �ε�
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if(TempGunMesh.Succeeded())
	{
		//4-4 ���̷�Ż �޽� ������ �Ҵ�
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		//4-5 ��ġ �����ϱ�
		gunMeshComp->SetRelativeLocation(FVector(-14, 52, 120));
	}

	//5.�������� �� ���۳�Ʈ ���
	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunComp"));
	
	//5-1. �θ� ���۳�Ʈ�� Mesh���۳�Ʈ�� ����
	sniperGunComp->SetupAttachment(GetMesh());

	//5-2. ����ƽ�޽� ������ �ε�
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("StaticMesh'/Game/SniperGun/sniper1.sniper1'"));

	//5-3. ������ �ε尡 �����ߴٸ�
	if (TempSniperMesh.Succeeded())
	{
		//5-4. ����ƽ�޽� ������ �Ҵ�
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);

		//5-5. ��ġ �����ϱ�
		sniperGunComp->SetRelativeLocation(FVector(-22, 55, 120));

		//5-6. ũ�� �����ϱ�
		sniperGunComp->SetRelativeScale3D(FVector(0.15f));
	}


}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		if (subsystem)
		{
			subsystem->AddMappingContext(imc_TPS, 0);
		}
	}
	
	//1. �������� UI ���� �ν��Ͻ� ����
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	//2. �Ϲ� ���� UIũ�ν���� �ν��Ͻ� ����
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);
	//3.�Ϲ� ���� UI ���
	_crosshairUI->AddToViewport();

	ChangeToSniperGun(FInputActionValue());

	
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlayerMove();
}

void ATPSPlayer::PlayerMove()
{
	//�÷��̾� �̵�ó��
	//��� �
	//P(��� ��ġ) = P0(���� ��ġ) + V(�ӵ�) X T(�ð�)
	//direction = FTransform(GetControlRotation()).TransformVector(direction);
	//FVector P0 = GetActorLocation();
	//FVector vt = direction * walkSpeed * DeltaTime;
	//FVector P = P0 + vt;
	//SetActorLocation(P);
	//�ش� ������ ���� ���ִ� �Լ��� ���������
	direction = FTransform(GetControlRotation()).TransformVector(direction);
	AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (PlayerInput)
	{
		PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &ATPSPlayer::Turn);
		PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &ATPSPlayer::LookUp);
		PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &ATPSPlayer::Move);
		//������ ���ε� ��ų�� Ʈ�� �޽� �̱⶧���� Started�� ���ش�.
		PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this, &ATPSPlayer::InputJump);
		//PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this, &ATPSPlayer::Fire);
		//�Ѿ� �߻� �̺�Ʈ ó���Լ� ���ε�
		PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this, &ATPSPlayer::InputFire);
		//�� ��ü �̺�Ʈ ó�� �Լ� ���ε�
		PlayerInput->BindAction(ia_GrenadeGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToGrenadeGun);
		PlayerInput->BindAction(ia_SniperGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToSniperGun);
		
		//�������� ���� ��� �̺�Ʈ ó�� �Լ� ���ε�
		PlayerInput->BindAction(ia_Sniper, ETriggerEvent::Started, this, &ATPSPlayer::SnimperAim);
		PlayerInput->BindAction(ia_Sniper, ETriggerEvent::Completed, this, &ATPSPlayer::SnimperAim);
	}


}

void ATPSPlayer::SnimperAim(const struct FInputActionValue& inputValue)
{
	if (bUsingGrenadeGun)
	{
		return;
	}
	//Pressed �Է� ó��
	if (bSniperAim == false)
	{
		//1.�������� ���� ��� Ȱ��ȭ
		bSniperAim = true;
		//2. �������� ���� UI ���
		_sniperUI->AddToViewport();
		//3. ī�޶��� �þ߰� Field Of View ����
		tpsCamComp->SetFieldOfView(45.0f);
		//4. �Ϲ� ���� UI����
		_crosshairUI->RemoveFromParent();
	}
	//Released �Է� ó��
	else
	{
		//1.�������� ���� ��� ��Ȱ��ȭ
		bSniperAim = false;
		//2.�������� ���� UI ȭ�鿡�� ����
		_sniperUI->RemoveFromParent();
		//3. ī�޶� �þ߰� ������� �ǵ����� // �̰� �������� ����ص״ٰ� �Է¸� �ϵ��� �ص��ɵ�?
		tpsCamComp->SetFieldOfView(90.0f);
		//4. �Ϲ� ���� UI ���
		_crosshairUI->AddToViewport();
	}
}

void ATPSPlayer::ChangeToGrenadeGun(const struct FInputActionValue& inputValue)
{
	//��ź������ ����
	//��ź�� ��� ������ üũ
	bUsingGrenadeGun = true;
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);
}

void ATPSPlayer::ChangeToSniperGun(const struct FInputActionValue& inputValue)
{
	//�������� ������ ����
	//��ź�� ��� x�� ����
	bUsingGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
}
void ATPSPlayer::InputFire(const struct FInputActionValue& inputValue)
{
	//��ź�� ��� ��
	if (bUsingGrenadeGun)
	{
		//�Ѿ� �߻� ó��
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
	//�������� �� ��� ��
	else
	{
		//LineTrace�� ���� ��ġ
		FVector startPos = tpsCamComp->GetComponentLocation();
		//LineTrace�� ���� ��ġ
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;
		//LineTrace�� �浹 ������ ���� ����
		FHitResult hitInfo;
		//�浹 �ɼ� ���� ����
		FCollisionQueryParams params;
		//�ڱ� �ڽ�(�÷��̾�)�� �浹���� ����
		params.AddIgnoredActor(this);
		//Channel���͸� �̿��� LineTrace�浹 ����(�浹 ����, ������ġ, ������ġ, ����ä��, �浹�ɼ�)
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
		//LineTrace�� �ε����� ��
		if (bHit)
		{
			//�Ѿ� ���� ȿ�� Ʈ������
			FTransform bulletTrans;
			//�ε��� ��ġ �Ҵ�
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			//�Ѿ� ���� ȿ�� �ν��Ͻ� ����
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

			auto hitComp = hitInfo.GetComponent();
			//1. ���� ���۳�Ʈ�� ������ ����Ǿ� �ִٸ�
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				//2. ������ ������ �ʿ�
				FVector dir = (endPos - startPos).GetSafeNormal();
				//���� ���� ��(F=ma)
				FVector force = dir * hitComp->GetMass() * 500000;
				//3. �� �������� ���� ������ʹ�.
				hitComp->AddForceAtLocation(force, hitInfo.ImpactPoint);
			}

			//부딪힌 대상이 적인지 판단하기
			auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
			if (enemy)
			{
				auto enemyFSM = Cast<UEnemyFSM>(enemy);
				enemyFSM->OnDamageProcess();
			}
		}
	}
}
void ATPSPlayer::Turn(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerYawInput(value);
}

void ATPSPlayer::LookUp(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerPitchInput(value);
}

void ATPSPlayer::Move(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();
	//���� �Է� �̺�Ʈ ó��
	direction.X = value.X;
	//�¿� �Է� �̺�Ʈ ó��
	direction.Y = value.Y;
}

void ATPSPlayer::InputJump(const FInputActionValue& inputValue)
{
	Jump();
}
//
//void ATPSPlayer::Fire(const FInputActionValue& inputValue)
//{
//
//}
