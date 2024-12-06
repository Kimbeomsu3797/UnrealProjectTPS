// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

//사용할 상태 정의
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Move,
	Attack,
	Damage,
	Die,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSPROJECT_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();
	
	//기본속성으로 상태를 idle로 설정해둔다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSM)
		EEnemyState mState = EEnemyState::Idle;
	//대기상태
	void IdleState();
	//이동상태
	void MoveState();
	//공격상태
	void AttackState();
	//피격상태
	void DamageState();
	//죽음상태
	void DieState();

	//대기 상태
	UPROPERTY(EditDefaultsOnly, Category = FSM)
	//대기 시간
	float idleDelayTime = 2;
	//경과 시간
	float currentTime = 0;

	//타깃
	UPROPERTY(VisibleAnywhere, Category = FSM)
		class ATPSPlayer* target;

	//소유 액터
	UPROPERTY()
		class AEnemy* me;

	//공격 가능 범위
	UPROPERTY(EditAnywhere, Category = FSM)
		float attackRange = 150.0f;

	//공격 대기 시간
	UPROPERTY(EditAnywhere, Category = FSM)
		float attackDelayTime = 2.0f;

	//체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
		int32 hp = 3;

	//피격 대기 시간 선언
	UPROPERTY(EditAnywhere, Category = FSM)
		float damageDelayTime = 2.0f;

	//아래로 사라지는 속도를 설정
	UPROPERTY(EditAnywhere, Category = FSM)
		float dieSpeed = 50.0f;
	//피격 알림 이벤트 함수 선언
	void OnDamageProcess();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
