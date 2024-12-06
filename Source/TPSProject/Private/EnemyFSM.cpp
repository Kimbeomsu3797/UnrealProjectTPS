// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "TPSPlayer.h"
#include "Enemy.h"
#include <Kismet/GameplayStatics.h>
#include "TPSProject.h"
#include <Components/CapsuleComponent.h>

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	//월드에서 ATPSPlayer 타깃 찾아오기
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
	//ATPSPlayer 타입으로 캐스팅
	target = Cast<ATPSPlayer>(actor);
	//소유 객체 가져오기
	me = Cast<AEnemy>(GetOwner());
}

// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	default:
		break;
	}

	FString logMsg = UEnum::GetValueAsString(mState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, logMsg);
}

//아이들
void UEnemyFSM::IdleState()
{
	//1. 시간이 흘렀으니까
	currentTime += GetWorld()->DeltaTimeSeconds;
	//2. 만약 경과 시간이 대기 기산을 초과했다면
	if (currentTime > idleDelayTime)
	{
		//3. 이동 상태로 전환
		mState = EEnemyState::Move;
		//경과 시간 초기화
		currentTime = 0;
	}
}

//이동
void UEnemyFSM::MoveState()
{
	//1. 타깃 목적지를 할당
	FVector destination = target->GetActorLocation();
	//2. 방향이 필요
	FVector dir = destination - me->GetActorLocation();
	//3. 구한 방향으로 이동하고 싶다.
	me->AddMovementInput(dir.GetSafeNormal());
	//타깃과 가까워지면 공격 상태로 변경
	if (dir.Size() < attackRange)
	{
		mState = EEnemyState::Attack;
	}
}

//공격
void UEnemyFSM::AttackState()
{
	//목표: 일정 시간에 한 번씩 공격하게 한다.
	//1. 시간이 흘러야 한다.
	currentTime += GetWorld()->DeltaTimeSeconds;
	//2. 공격 시간이 됐으니까
	if (currentTime > attackDelayTime)
	{
		//3. 공격하고 싶다.
		PRINT_LOG(TEXT("Attack!!"));
		//경과 시간 초기화
		currentTime = 0;
	}

	//1. 타깃과의 거리가 필요
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
	//2. 타깃과의 거리가 공격 범위를 벗어났으니까
	if (distance > attackRange)
	{
		//3. 상태를 이동으로 전환
		mState = EEnemyState::Move;
	}

}

//피격
void UEnemyFSM::DamageState()
{
	//1. 시간이 흘렀으니까
	currentTime += GetWorld()->DeltaTimeSeconds;
	//2. 경과 시간이 대기 시간을 초과 했다면
	if (currentTime > damageDelayTime)
	{
		//3. 상태를 아이들로 변경
		mState = EEnemyState::Idle;
		//경과 시간 초기화
		currentTime = 0;
	}
}

void UEnemyFSM::OnDamageProcess()
{
	//체력 감소
	hp--;
	//체력이 남아있다면
	if (hp > 0)
	{
		//피격상태로 전환
		mState = EEnemyState::Damage;
	}
	//체력이 없다면
	else
	{
		//죽은 상태로 변경
		mState = EEnemyState::Die;
		//캡슐 충돌체 비활성화
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

//죽음
void UEnemyFSM::DieState()
{
	//계속 아래로 내려가고 싶다..
	//등속운동 공식 P=P0+vt
	FVector P0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaRealTimeSeconds;
	FVector p = P0 + vt;
	me->SetActorLocation(p);

	if (p.Z < -200.0f)
	{
		me->Destroy();
	}
}