// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

//����� ���� ����
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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	//���� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSM)
		EEnemyState mState = EEnemyState::Idle;

	//��� ����
	void IdleState();
	//�̵� ����
	void MoveState();
	//���� ����
	void AttackState();
	//�ǰ� ����
	void DamagedState();
	//���� ����
	void DieState();

	//��� �ð�
	UPROPERTY(EditDefaultsOnly, Category = FSM)
		float idleDelayTime = 2;
	//��� �ð�
	float currentTime = 0;

	//Ÿ��
	UPROPERTY(VisibleAnywhere, Category = FSM)
		class ATPSPlayer* target;
	//���� ����
	UPROPERTY()
		class AEnemy* me;

	//���� ����
	UPROPERTY(EditAnywhere, Category = FSM)
		float attackRange = 150.0f;

	//���� ��� �ð�
	UPROPERTY(EditAnywhere, Category = FSM)
		float attackDelayTime = 2.0f;

	//피격 알림 이벤트 함수
	void OnDamageProcess();

	//체력
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = FSM)
	int32 hp =3;

	//피격 대기 시간
	UPROPERTY(EditAnywhere, Category = FSM)
	float damageDelayTime = 2.0f;

	//아래로 사라지는 속도
	UPROPERTY(EditAnywhere, Category = FSM)
	float dieSpeed = 50.0f;
};
