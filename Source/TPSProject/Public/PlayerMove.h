// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseComponent.h"
#include "PlayerMove.generated.h"

/**
 * 
 */
UCLASS()
class TPSPROJECT_API UPlayerMove : public UPlayerBaseComponent
{
	GENERATED_BODY()

public:
	//ȸ�� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_LookUp;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Turn;
	//�¿� ȸ�� �Է� ó��
	void Turn(const struct FInputActionValue& inputValue);
	//���� ȸ�� �Է� ó��
	void LookUp(const struct FInputActionValue& inputValue);

	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput) override;

	//�̵� �Է� ó��
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Move;
	//�̵� ����
	FVector direction;
	//�̵� ���� �Լ�
	void Move(const struct FInputActionValue& inputValue);
	//�̵� �ӵ� ó��
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float walkSpeed = 200;
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float runSpeed = 600;
	//���� �̵� �Լ��� �ۼ�
	void PlayerMove();
};
