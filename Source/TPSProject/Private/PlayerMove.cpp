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
	PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &UPlayerMove::Move);
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

//�̵� ó�� �Լ�
void UPlayerMove::Move(const FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();
	//���� �Է� �̺�Ʈ ó��
	direction.X = value.X;
	//�¿� �Է� �̺�Ʈ ó��
	direction.Y = value.Y;
}

//���� �̵� �Լ�
void UPlayerMove::PlayerMove()
{
	//�÷��̾� �̵�ó��
	//��� �
	//P(��� ��ġ) = P0(���� ��ġ) + V(�ӵ�) X T(�ð�)
	//direction = FTransform(GetControlRotation()).TransformVector(direction);
	//FVector P0 = GetActorLocation();
	//FVector vt = direction * walkSpeed * DeltaTime;
	//FVector P = P0 + vt;
	//SetActorLocation(P);
	//�ش� ������ ���� ���ִ� �Լ��� ���������
	direction = FTransform(me->GetControlRotation()).TransformVector(direction);
	me->AddMovementInput(direction);
	direction = FVector::ZeroVector;
}
