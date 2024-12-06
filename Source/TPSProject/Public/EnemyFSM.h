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
	
	//�⺻�Ӽ����� ���¸� idle�� �����صд�.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSM)
		EEnemyState mState = EEnemyState::Idle;
	//������
	void IdleState();
	//�̵�����
	void MoveState();
	//���ݻ���
	void AttackState();
	//�ǰݻ���
	void DamageState();
	//��������
	void DieState();

	//��� ����
	UPROPERTY(EditDefaultsOnly, Category = FSM)
	//��� �ð�
	float idleDelayTime = 2;
	//��� �ð�
	float currentTime = 0;

	//Ÿ��
	UPROPERTY(VisibleAnywhere, Category = FSM)
		class ATPSPlayer* target;

	//���� ����
	UPROPERTY()
		class AEnemy* me;

	//���� ���� ����
	UPROPERTY(EditAnywhere, Category = FSM)
		float attackRange = 150.0f;

	//���� ��� �ð�
	UPROPERTY(EditAnywhere, Category = FSM)
		float attackDelayTime = 2.0f;

	//ü��
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
		int32 hp = 3;

	//�ǰ� ��� �ð� ����
	UPROPERTY(EditAnywhere, Category = FSM)
		float damageDelayTime = 2.0f;

	//�Ʒ��� ������� �ӵ��� ����
	UPROPERTY(EditAnywhere, Category = FSM)
		float dieSpeed = 50.0f;
	//�ǰ� �˸� �̺�Ʈ �Լ� ����
	void OnDamageProcess();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	class UEnemyAnim* anim;
};
