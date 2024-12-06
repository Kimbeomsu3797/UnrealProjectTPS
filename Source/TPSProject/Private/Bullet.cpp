// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include <Components/SphereComponent.h>
#include <Components/StaticMeshComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//1. 충돌체 등록
	collisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollComp"));
	//2. 충돌프로파일 설정
	collisionComp->SetCollisionProfileName(TEXT("BlockAll"));
	//3. 충돌체 크기 설정
	collisionComp->SetSphereRadius(13);
	//4. 루트로 등록
	RootComponent = collisionComp;
	//5. 외관 컴포넌트 등록
	bodyComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComp"));
	//6. 부모 컴포넌트 지정
	bodyComp->SetupAttachment(collisionComp);
	//7. 충돌 비활성화
	bodyComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//8. 외간 크기 설정
	bodyComp->SetRelativeScale3D(FVector(0.25f));
	//발사체 컴포넌트
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	//movemonet 컴포넌트가 갱신시킬 컴포넌트 지정
	movementComp->SetUpdatedComponent(collisionComp);
	//속도를 한번에 처리하기위한 기능을 작성해줄 것이다. 
	
 
	//초기속도
	//movementComp->InitialSpeed = 5000;
	//최대속도
	//movementComp->MaxSpeed = 5000;
	//반동 여부
	movementComp->bShouldBounce = true;
	//반동 값
	movementComp->Bounciness = 0.3f;

	//인스턴스 삭제 1
	//생명 시간 주기
	//InitialLifeSpan = 2.0f;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	//Die함수를 생성하여 2초뒤에 삭제되게끔 하는 함수를 제작할 수도 있다.
	FTimerHandle deathTimer;
	//인스턴스 삭제 2
	//GetWorld()->GetTimerManager().SetTimer(deathTimer, this, &ABullet::Die, 2.0f, false);
	//인스턴스 삭제 3.
	GetWorld()->GetTimerManager().SetTimer(deathTimer, FTimerDelegate::CreateLambda([this]()->void
		{
			Destroy();
		}), 2.0f, false);
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == TEXT("speed"))
	{
		movementComp->InitialSpeed = speed;
		movementComp->MaxSpeed = speed;
	}
}

//인스턴스 삭제 2(h파일에 함수 선언 해줘야함)
//void ABullet::Die()
//{
//	Destroy();
//}
