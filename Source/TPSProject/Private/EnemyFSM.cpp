// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "TPSPlayer.h"
#include "Enemy.h"
#include <Kismet/GameplayStatics.h>
#include "TPSProject.h"
#include <Components//CapsuleComponent.h>
// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// ...
	//���忡�� ATPSPlayer Ÿ�� ã�ƿ���
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
	//ATPSPlayerŸ������ ĳ����
	target = Cast<ATPSPlayer>(actor);
	//���� ��ü ��������
	me = Cast<AEnemy>(GetOwner());
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//����â�� ���� �޼��� ����ϱ�
	FString logMsg = UEnum::GetValueAsString(mState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, logMsg);

	switch (mState)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamagedState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	default:
		break;
	}
	// ...
}

//��� ����
void UEnemyFSM::IdleState() 
{
	//1. �ð��� �귶���ϱ�
	currentTime += GetWorld()->DeltaTimeSeconds;
	//2. ���� ��� �ð��� ��� �ð��� �ʰ��ߴٸ�
	if (currentTime > idleDelayTime)
	{
		//3. �̵� ���·� ��ȯ�ϰ� �ʹ�.
		mState = EEnemyState::Move;
		//��� �ð� �ʱ�ȭ
		currentTime = 0;
	}
}

//�̵� ����
void UEnemyFSM::MoveState() 
{
	//1. Ÿ���� �������� �ʿ��ϴ�.
	FVector destination = target->GetActorLocation();
	//2. ������ �ʿ��ϴ�
	FVector dir = destination - me->GetActorLocation();
	//3. �������� �̵��ϰ� �ʹ�.
	me->AddMovementInput(dir.GetSafeNormal());

	//Ÿ�ϰ� ��������� ���� ���·� ��ȯ�ϰ� �ʹ�.
	//1.���� �Ÿ��� ���� ���� �ȿ� ������
	if (dir.Size() < attackRange)
	{
		//2.���� ���·� ��ȯ�ϰ� �ʹ�.
		mState = EEnemyState::Attack;
	}
}

//���� ����
void UEnemyFSM::AttackState() 
{
	//��ǥ: ���� �ð��� �� ���� �����ϰ� �ʹ�.
	//1. �ð��� �귯�� �Ѵ�.
	currentTime += GetWorld()->DeltaTimeSeconds;
	//2. ���� �ð��� �����ϱ�
	if (currentTime > attackDelayTime)
	{
		//3. �����ϰ� �ʹ�.
		PRINT_LOG(TEXT("Attack!!!!!"));
		//��� �ð� �ʱ�ȭ
		currentTime = 0;
	}
	
	//목표: 타깃이 공격 범위를 벗어나면 상태를 이동으로 전환하고 싶다.
	//1. 타깃과의 거리가 필요하다.
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
	//2. 타깃과의 거리가 공격 범위를 벗어났으니까
	if (distance > attackRange)
	{
		//3. 상태를 이동으로 전환하고 싶다.
		mState = EEnemyState::Move;
	}
}

void UEnemyFSM::OnDamageProcess()
{

	//체력 감소
	hp--;
	//만약 체력이 남아있다면
	if (hp>0)
	{
		//상태를 피격으로 전환
		mState = EEnemyState::Damage;
	}
	//그렇지 않다면
	else
	{
		//상태를 죽음으로 전환
		mState = EEnemyState::Die;
		//캡슐 충돌체 비활성화
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	
}

//�ǰ� ����
void UEnemyFSM::DamagedState()
{
	//1. 시간이 흘렀으니까
	currentTime += GetWorld()->DeltaTimeSeconds;
	//2. 만약 경과 시간이 대기 시간을 초과했다면
	if (currentTime > damageDelayTime)
	{
		//3. 대기 상태로 전환하고 싶다.
		mState = EEnemyState::Idle;
		//경과 시간 초기화
		currentTime = 0;
	}
}

//���� ����
void UEnemyFSM::DieState()
{
	//계속 아래로 내려가고 싶다.
	//등속운동 공식 P = P0 + vt
	FVector P0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = P0 + vt;
	me->SetActorLocation(P);

	//1. 만약 2미터 이상 내려왔다면
	if (P.Z < -200.0f)
	{
		//2. 제거시킨다.
		me->Destroy();
	}
}
