// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakables/Breakable.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Items/Treasure.h"

// Sets default values
ABreakable::ABreakable() : bBroken(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);		// 기본적으로 오버랩 이벤트를 활성화하는 변수 Setter
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule_"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

// Called when the game starts or when spawned
void ABreakable::BeginPlay()
{
	Super::BeginPlay();
	
	GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakable::OnChaosBreakEvent);
}

// Called every frame
void ABreakable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakable::GetHit_Implementation(const FVector& ImpactPoint)
{
	
	//GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); <- 여기서 이걸 써도 파편에 제대로 적용이 안 됨
}

void ABreakable::OnChaosBreakEvent(const FChaosBreakEvent& BreakEvent)	// 카오스 물리엔진 파괴 이벤트 델리게이트 함수, 물체가 어떤 이유로든 부서질 때 실행
{
	if (bBroken)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World && TreasureClasses.Num() > 0)
	{
		FVector Location = GetActorLocation();
		Location.Z += 75.f;						// 위치 보정

		int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);	// 벡터 내 원소를 잘못된 인덱스 접근이 없게 랜덤 접근하기 위한 랜덤 변수

		World->SpawnActor<ATreasure>(TreasureClasses[Selection], Location, GetActorRotation());	// C++ 클래스는 메시 정보같은 언리얼에서 설정해야 하는 정보가 없기 때문에 
		// 매개변수로 UClass라는 것을 받아 언리얼에서 만든 블루프린트 클래스도 선택할 수 있게 해준다.
	}

	bBroken = true;
	SetLifeSpan(5.f);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

