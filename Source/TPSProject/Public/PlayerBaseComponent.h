// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Components/ActorComponent.h"
#include "PlayerBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSPROJECT_API UPlayerBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerBaseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	//사용자 입력 매핑 처리 함수
	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput){};
	
	//컴퍼넌트 소유 액터
	UPROPERTY()
	ATPSPlayer* me;
	UPROPERTY()
	UCharacterMovementComponent* moveComp;
		
};
