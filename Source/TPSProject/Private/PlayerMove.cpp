// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMove.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"



void UPlayerMove::SetupInputBinding(class UEnhancedInputComponent* PlayerInput)
{
	//�̵� ���� �̺�Ʈ ó�� �Լ� ���ε�
	PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &UPlayerMove::Turn);
	PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &UPlayerMove::LookUp);
}

//�¿� ȸ�� �Լ�
void UPlayerMove::Turn(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	me->AddControllerYawInput(value);
}

//���Ʒ� ȸ�� �Լ�
void UPlayerMove::LookUp(const FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	me->AddControllerPitchInput(value);
}


