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

// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//1.스켈레탈메시 데이터를 불러오고싶다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		//2. Mesh 컴포넌트의 위치를 설정하고 싶다.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	//3.TPS 카메라를 붙이고 싶다.
	//3-1. SpringArm컴포넌트 붙이기
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));
	springArmComp->TargetArmLength = 400;
	//해당 컨트롤러 로테이션을 트루로
	springArmComp->bUsePawnControlRotation = true;
	//3-2. Camera컴포넌트를 붙인다.

	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);
	//해당 컨트롤러는 Yaw만 트루로 설정 Yaw는 z축 회전 Roll은 X축 회전 Pich는 Y충 회전
	tpsCamComp->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = true;
	JumpMaxCount = 2;

	// 4. 총 스켈레탈메시 컴포넌트 등록
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	// 4-1. 부모 컴포넌트를 Mesh 컴포넌트로 설정
	gunMeshComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));

	// 4-2. 스켈레탈메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	if(TempGunMesh.Succeeded())
	{
		//4-4 스켈레탈 메시 데이터 할당
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		//4-5 위치 조정하기
		gunMeshComp->SetRelativeRotation(FRotator(-17, 0, -3));
		gunMeshComp->SetRelativeLocation(FVector(0, 90, 0));
	}

	//5. 스나이퍼건 컴포넌트 등록
	sniperGunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SniperGunComp"));
	//5-1. 부모 컴포넌트를 Mesh 컴포넌트로 설정
	sniperGunComp->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));

	//5-2. 해당 컴포넌트의 경로를 로드해준다
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSniperMesh(TEXT("StaticMesh'/Game/SniperGun/sniper1.sniper1'"));
	//5-3. 로드에 성공했다면
	if (TempSniperMesh.Succeeded())
	{
		//5-4. 스태틱메시 데이터 할당
		sniperGunComp->SetStaticMesh(TempSniperMesh.Object);
		//5-5. 위치 조정
		sniperGunComp->SetRelativeRotation(FRotator(-42, 7, 1));
		sniperGunComp->SetRelativeLocation(FVector(0, 90, 0));
		//5-6 크기 조정하기
		sniperGunComp->SetRelativeScale3D(FVector(0.15f));
	}

	ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("SoundWave'/Game/SniperGun/Rifle.Rifle'"));
	
	if (tempSound.Succeeded())
	{
		bulletSound = tempSound.Object;
	}
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	//초기 속도를 걷기로 설정해준다.
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

	//1. 스나이퍼 UI위젯 인스턴스 생성
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);
	//2. 일반 조준 UI 크로스헤어 인스턴스 생성
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);
	//3. 일반 조준 UI를 등록
	_crosshairUI->AddToViewport();

	//기본적으로 스나이퍼를 착용하도록 설정
	ChangeToSniperGun(FInputActionValue());
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PlayerMove();
}

//실제 이동 함수
void ATPSPlayer::PlayerMove()
{
	//플레이어 이동처리
	//등속 운동
	//P(결과 위치) = P0(현재 위치) + V(속도) X T(시간)
	//direction = FTransform(GetControlRotation()).TransformVector(direction);
	//FVector P0 = GetActorLocation();
	//FVector vt = direction * walkSpeed * DeltaTime;
	//FVector P = P0 + vt;
	//SetActorLocation(P);
	//해당 내용이 전부 들어가있는 함수로 실행시켜줌
	direction = FTransform(GetControlRotation()).TransformVector(direction);
	AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

//인풋 바인드 부분
// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (PlayerInput)
	{
		//이동 관련 이벤트 처리 함수 바인딩
		PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &ATPSPlayer::Turn);
		PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &ATPSPlayer::LookUp);
		PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &ATPSPlayer::Move);

		//점프 이벤트 처리 함수 바인딩
		PlayerInput->BindAction(ia_Jump, ETriggerEvent::Started, this, &ATPSPlayer::InputJump);

		//발사 이벤트 처리 함수 바인딩
		PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this, &ATPSPlayer::InputFire);

		//각 총 교체 이벤트 처리 함수 바인딩
		PlayerInput->BindAction(ia_GrenadeGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToGrenadeGun);
		PlayerInput->BindAction(ia_SniperGun, ETriggerEvent::Started, this, &ATPSPlayer::ChangeToSniperGun);

		//스나이퍼 조준 처리 함수 바인딩
		PlayerInput->BindAction(ia_Sniper, ETriggerEvent::Started, this, &ATPSPlayer::SniperAim);
		PlayerInput->BindAction(ia_Sniper, ETriggerEvent::Completed, this, &ATPSPlayer::SniperAim);

		//달리기 입력 이벤트 처리 함수 바인딩
		PlayerInput->BindAction(ia_Run, ETriggerEvent::Started, this, &ATPSPlayer::InputRun);
		PlayerInput->BindAction(ia_Run, ETriggerEvent::Completed, this, &ATPSPlayer::InputRun);
	}
}

//좌우 회전 함수
void ATPSPlayer::Turn(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerYawInput(value);
}

//위아래 회전 함수
void ATPSPlayer::LookUp(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	AddControllerPitchInput(value);
}

//이동 처리 함수
void ATPSPlayer::Move(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();
	//상하 입력 이벤트 처리
	direction.X = value.X;
	//좌우 입력 이벤트 처리
	direction.Y = value.Y;
}

//점프 함수
void ATPSPlayer::InputJump(const FInputActionValue& inputValue)
{
	Jump();
}

void ATPSPlayer::InputRun()
{
	auto movement = GetCharacterMovement();
	//현재 달리기 모드라면 즉, 걷기 속도가 MaxWalkSpeed보다 작다면
	if (movement->MaxWalkSpeed > walkSpeed)
	{
		//걷기 속도로 전환
		movement->MaxWalkSpeed = walkSpeed;
	} else
	{
		movement->MaxWalkSpeed = runSpeed;
	}
}

//발사 함수
void ATPSPlayer::InputFire(const FInputActionValue& inputValue)
{

	//카메라 셰이크 재생
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(cameraShake);
	//총알 발사 사운드 재생
	UGameplayStatics::PlaySound2D(GetWorld(), bulletSound);

	//공격 에니메이션 재생
	auto anim = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	anim->PlayAttackAnim();

	//총알 발사 처리
	if (bUsingGrenadeGun)
	{
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	} 
	//스나이퍼건 사용 시
	else
	{
		//LineTrace의 시작 위치
		FVector startPos = tpsCamComp->GetComponentLocation();
		//LineTrace의 종료 위치
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp ->GetForwardVector() * 5000;
		//LineTrace의 충돌 정보를 담을 변수
		FHitResult hitInfo;
		//충동 옵션 설정 변수
		FCollisionQueryParams params;
		//자기 자신 은 충돌에서 제외(플레이어)
		params.AddIgnoredActor(this);

		//Channel필터를 이용한 LineTrace충돌 검출(충돌 정보, 시작 위치, 종료 위치, 검출 채널, 충돌 옵션)
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
		//LineTrace가 부딛혔을 때
		if (bHit)
		{
			//충돌 처리 -> 총알 파편 효과 재생
			//퐁알 파편 효과 트랜스폼
			FTransform bulletTrans;
			//부딪힌 위치 할당
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			//총알 파편 효과 인스턴스 생성
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);
		}
		auto hitComp = hitInfo.GetComponent();
		//1. 컴포넌트에 물리가 적용되어 있다면..
		if (hitComp && hitComp->IsSimulatingPhysics())
		{
			//2. 조준한 방향이 필요
			FVector dir = (endPos - startPos).GetSafeNormal();
			//날려버릴 힘(F=ma)
			FVector force = dir * hitComp->GetMass() * 500000;
			//3. 그방향으로 날리기
			hitComp->AddForceAtLocation(force, hitInfo.ImpactPoint);
		}

		//부딪힌 대상이 적인지 판단
		auto enemy = hitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
		if (enemy)
		{
			auto enemyFSM = Cast<UEnemyFSM>(enemy);
			enemyFSM->OnDamageProcess();
		}
	}
}

//기본 총 호출 함수
void ATPSPlayer::ChangeToGrenadeGun(const FInputActionValue& inputValue)
{
	bUsingGrenadeGun = true;
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);
}

//스나이퍼 건 호출 함수
void ATPSPlayer::ChangeToSniperGun(const FInputActionValue& inputValue)
{
	bUsingGrenadeGun = false;
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
}

//스나이퍼 모드 함수
void ATPSPlayer::SniperAim(const struct FInputActionValue& inputValue)
{
	//일반 총이면 리턴 시킨다.
	if (bUsingGrenadeGun) { return; }
	//Pressed 입력처리
	if(bSniperAim == false)
	{
		//1. 스나이퍼 조준 모드 활성화
		bSniperAim = true;
		//2. 스나이퍼 조준UI등록
		_sniperUI->AddToViewport();
		//3. 카메라의 시야각 Field Of View를 설정
		tpsCamComp->SetFieldOfView(45.0f);
		//4. 일반 조준 UI제거
		_crosshairUI->RemoveFromParent();
	} 
	//UnPressed 입력 처리
	else
	{
		//1. 스나이퍼 조준 모드 비활성화
		bSniperAim = false;
		//2. 스나이퍼 조준 UI 제거
		_sniperUI->RemoveFromParent();
		//3. 카메라 시야각 원래대로 복권
		tpsCamComp->SetFieldOfView(90.0f);
		//4. 일반 조준 UI 등록
		_crosshairUI->AddToViewport();
	}
}