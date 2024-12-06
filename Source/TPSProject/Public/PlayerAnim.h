// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class TPSPROJECT_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()
public:
	//플레이어 앞뒤 이동 속도 선언
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
		float speed = 0;
	//플레이어 좌우 이동 속도 선언
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
		float direction = 0;

	//매프레임 갱신되는 함수 오버라이딩 
	//오버라이딩은 cpp에서 반드시 구현 해줘야 함 그래야 빌드가 가능해짐.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	//플레이어가 공중에 있는지 여부 판단 선언
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
		bool isInAir = false;

	//재생할 공격 에니메이션 몽타주
	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
		class UAnimMontage* attackAnimMontage;

	//공격 애니메이션 재생 함수
	void PlayAttackAnim();
};
