// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class TPSPROJECT_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//발사체 이동 컴포넌트
	UPROPERTY(VisibleAnywhere, Category=Movement)
		class UProjectileMovementComponent* movementComp;
	UPROPERTY(VisibleAnywhere, Category=Collision)
		class USphereComponent* collisionComp;
	UPROPERTY(VisibleAnywhere, Category=BodyMesh)
		class UStaticMeshComponent* bodyComp;
	//총알 제거 함수
	//void Die();
};
