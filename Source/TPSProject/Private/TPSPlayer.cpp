// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Camera/CameraComponent.h>
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Bullet.h"
#include <Components/StaticMeshComponent.h>
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>
#include "Enemy.h"
#include "EnemyFSM.h"
#include "PlayerAnim.h"
#include "PlayerMove.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//1.���̷�Ż�޽� �����͸� �ҷ�����ʹ�.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		//2. Mesh ������Ʈ�� ��ġ�� �����ϰ� �ʹ�.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	//3.TPS ī�޶� ���̰� �ʹ�.
	//3-1. SpringArm������Ʈ ���̱�
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
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
	gunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));

	// 4-2. ���̷�Ż�޽� ������ �ε�
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if(TempGunMesh.Succeeded())
	{
		//4-4 ���̷�Ż �޽� ������ �Ҵ�
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		//4-5 ��ġ �����ϱ�
		gunMeshComp->SetRelativeRotation(FRotator(-17, 0, -3));
		gunMeshComp->SetRelativeLocation(FVector(0, 90, 0));
	}

	//5. �������۰� ������Ʈ ���
	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunComp"));
	//5-1. �θ� ������Ʈ�� Mesh ������Ʈ�� ����
	sniperGunComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));

	//5-2. �ش� ������Ʈ�� ��θ� �ε����ش�
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("StaticMesh'/Game/SniperGun/sniper1.sniper1'"));
	//5-3. �ε忡 �����ߴٸ�
	if (TempSniperMesh.Succeeded())
	{
		//5-4. ����ƽ�޽� ������ �Ҵ�
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);
		//5-5. ��ġ ����
		sniperGunComp->SetRelativeRotation(FRotator(-42, 7, 1));
		sniperGunComp->SetRelativeLocation(FVector(0, 90, 0));
		//5-6 ũ�� �����ϱ�
		sniperGunComp->SetRelativeScale3D(FVector(0.15f));
	}

	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("SoundWave'/Game/SniperGun/Rifle.Rifle'"));
	
	if (tempSound.Succeeded())
	{
		bulletSound = tempSound.Object;
	}
	playerMove = CreateDefaultSubobject<UPlayerMove>(TEXT("PlayerMove"));
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	//�ʱ� �ӵ��� �ȱ�� �������ش�.
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;

	auto pc = Cast<APlayerController>(Controller);

	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		if (subsystem)
		{
			subsystem->AddMappingContext(imc_TPS, 0);
		}
	}

	//1. �������� UI���� �ν��Ͻ� ����
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);
	//2. �Ϲ� ���� UI ũ�ν���� �ν��Ͻ� ����
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);
	//3. �Ϲ� ���� UI�� ���
	_crosshairUI->AddToViewport();

	//�⺻������ �������۸� �����ϵ��� ����
	ChangeToSniperGun(FInputActionValue());
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlayerMove();
}



//��ǲ ���ε� �κ�
// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (PlayerInput)
	{
		//컴퍼넌트에서 입력 바인딩 처리하도록 호출
		playerMove->SetupInputBinding(PlayerInput);

		

		//���� �̺�Ʈ ó�� �Լ� ���ε�
		PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this, &ATPSPlayer::InputJump);

		//�߻� �̺�Ʈ ó�� �Լ� ���ε�
		PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this, &ATPSPlayer::InputFire);

		//�� �� ��ü �̺�Ʈ ó�� �Լ� ���ε�
		PlayerInput->BindAction(ia_GrenadeGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToGrenadeGun);
		PlayerInput->BindAction(ia_SniperGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToSniperGun);

		//�������� ���� ó�� �Լ� ���ε�
		PlayerInput->BindAction(ia_Sniper, ETriggerEvent::Started, this, &ATPSPlayer::SniperAim);
		PlayerInput->BindAction(ia_Sniper, ETriggerEvent::Completed, this, &ATPSPlayer::SniperAim);

		//�޸��� �Է� �̺�Ʈ ó�� �Լ� ���ε�
		PlayerInput->BindAction(ia_Run, ETriggerEvent::Started, this, &ATPSPlayer::InputRun);
		PlayerInput->BindAction(ia_Run, ETriggerEvent::Completed, this, &ATPSPlayer::InputRun);
	}
}





//���� �Լ�
void ATPSPlayer::InputJump(const FInputActionValue& inputValue)
{
	Jump();
}

void ATPSPlayer::InputRun()
{
	auto movement = GetCharacterMovement();
	//���� �޸��� ����� ��, �ȱ� �ӵ��� MaxWalkSpeed���� �۴ٸ�
	if (movement->MaxWalkSpeed > walkSpeed)
	{
		//�ȱ� �ӵ��� ��ȯ
		movement->MaxWalkSpeed = walkSpeed;
	} else
	{
		movement->MaxWalkSpeed = runSpeed;
	}
}

//�߻� �Լ�
void ATPSPlayer::InputFire(const FInputActionValue& inputValue)
{

	//ī�޶� ����ũ ���
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);
	//�Ѿ� �߻� ���� ���
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);

	//���� ���ϸ��̼� ���
	auto anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();

	//�Ѿ� �߻� ó��
	if (bUsingGrenadeGun)
	{
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	} 
	//�������۰� ��� ��
	else
	{
		//LineTrace�� ���� ��ġ
		FVector startPos = tpsCamComp->GetComponentLocation();
		//LineTrace�� ���� ��ġ
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp ->GetForwardVector() * 5000;
		//LineTrace�� �浹 ������ ���� ����
		FHitResult hitInfo;
		//�浿 �ɼ� ���� ����
		FCollisionQueryParams params;
		//�ڱ� �ڽ� �� �浹���� ����(�÷��̾�)
		params.AddIgnoredActor(this);

		//Channel���͸� �̿��� LineTrace�浹 ����(�浹 ����, ���� ��ġ, ���� ��ġ, ���� ä��, �浹 �ɼ�)
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
		//LineTrace�� �ε����� ��
		if (bHit)
		{
			//�浹 ó�� -> �Ѿ� ���� ȿ�� ���
			//���� ���� ȿ�� Ʈ������
			FTransform bulletTrans;
			//�ε��� ��ġ �Ҵ�
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			//�Ѿ� ���� ȿ�� �ν��Ͻ� ����
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);
		}
		auto hitComp = hitInfo.GetComponent();
		//1. ������Ʈ�� ������ ����Ǿ� �ִٸ�..
		if (hitComp && hitComp->IsSimulatingPhysics())
		{
			//2. ������ ������ �ʿ�
			FVector dir = (endPos - startPos).GetSafeNormal();
			//�������� ��(F=ma)
			FVector force = dir * hitComp->GetMass() * 500000;
			//3. �׹������� ������
			hitComp->AddForceAtLocation(force, hitInfo.ImpactPoint);
		}

		//�ε��� ����� ������ �Ǵ�
		auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
		if (enemy)
		{
			auto enemyFSM = Cast<UEnemyFSM>(enemy);
			enemyFSM->OnDamageProcess();
		}
	}
}

//�⺻ �� ȣ�� �Լ�
void ATPSPlayer::ChangeToGrenadeGun(const FInputActionValue& inputValue)
{
	bUsingGrenadeGun = true;
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);
}

//�������� �� ȣ�� �Լ�
void ATPSPlayer::ChangeToSniperGun(const FInputActionValue& inputValue)
{
	bUsingGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
}

//�������� ��� �Լ�
void ATPSPlayer::SniperAim(const struct FInputActionValue& inputValue)
{
	//�Ϲ� ���̸� ���� ��Ų��.
	if (bUsingGrenadeGun) { return; }
	//Pressed �Է�ó��
	if(bSniperAim == false)
	{
		//1. �������� ���� ��� Ȱ��ȭ
		bSniperAim = true;
		//2. �������� ����UI���
		_sniperUI->AddToViewport();
		//3. ī�޶��� �þ߰� Field Of View�� ����
		tpsCamComp->SetFieldOfView(45.0f);
		//4. �Ϲ� ���� UI����
		_crosshairUI->RemoveFromParent();
	} 
	//UnPressed �Է� ó��
	else
	{
		//1. �������� ���� ��� ��Ȱ��ȭ
		bSniperAim = false;
		//2. �������� ���� UI ����
		_sniperUI->RemoveFromParent();
		//3. ī�޶� �þ߰� ������� ����
		tpsCamComp->SetFieldOfView(90.0f);
		//4. �Ϲ� ���� UI ���
		_crosshairUI->AddToViewport();
	}
}