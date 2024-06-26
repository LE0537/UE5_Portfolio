// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Components/SphereComponent.h"
#include "Characters/SlashCharacter.h"
#include "test2/DebugMacro.h"

//Class Default Object (CDO) : 언리얼 엔진 리플렉션 시스템에 의해 생성되는 객체의 복사본,
// 엔진이 시작될 때나 컴파일 시 생성됨, 리플렉션 시스템이 우리 클래스에 기반한 블루프린트 객체들을 초기화할 때 필요한 값들을 보관하기 위해 생성됨
// 그래서 객체의 생성자에서 함수 등을 쓸 때 엔진의 모든 기능이 활성화되진 않았을 수 있으므로 주의해야 함
// 따라서 생성자가 아닌 BeginPlay 등에서 생성 시 처리할 것들을 실행해야 함

//컴포넌트에도 비슷한 현상이 일어남. 컴포넌트를 생성하기 위해선 디폴트 하위객체를 만들어야 함
// 디폴트 하위객체는 CDO와 비슷하지만 컴포넌트의 디폴트 정보를 가지고 있음
// 하위객체를 만들 때는 몇 가지 정보를 전달해야 함
// 
//		1. 객체 타입(UStaticMeshComponent 등)
//		2. 내부 이름(컴포넌트 변수 이름과 다름)
//		CreateDefaultSubobject<Type>(Name);
//		
//		이 함수는 새로 생성된 객체를 가리키는 포인터를 반환한다. 이런 함수를 팩토리라고 한다.
//		언리얼 엔진에서는 팩토리 함수를 많이 사용하기 때문에 new를 직접 사용하지 않는다

// Sets default values
AItem::AItem() : Amplitude(0.25f), TimeConstant(5.f), RunningTime(0.f), ItemState(EItemState::EIS_Hovering)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));	//디폴트 하위객체 생성 팩토리 함수
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

#pragma region Debug Logs

	//UE_LOG(LogTemp, Warning, TEXT("Begin Play Called!"));

	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(0, 60.f, FColor::Cyan, FString("Item OnScreen Message!"));
	//}

	//UWorld* World = GetWorld();
	//FVector Location = GetActorLocation();
	//FVector Forward = GetActorForwardVector();

	//if (World)
	//{
	//	DrawDebugLine(World, Location, Location + Forward * 100.f, FColor::Red, true, -1.f, 0, 1.f);
	//}

	//if (World)
	//{
	//	FVector Location = GetActorLocation();
	//	DrawDebugSphere(World, Location, 25.f, 24, FColor::Green, false, 30.f);
	//}

	//if (World)
	//{
	//	DrawDebugPoint(World, Location + Forward * 100.f, 15.f, FColor::Red, true);
	//}

	//DRAW_SPHERE(Location);
	//DRAW_LINE(Location, Location + Forward * 100.f);
	//DRAW_POINT(Location + Forward * 100.f);
	//DRAW_VECTOR(Location, Location + Forward * 100.f);

	//int32 AvgInt = Avg<int32>(1, 3);
	//UE_LOG(LogTemp, Warning, TEXT("Avg of 1 and 3: %d"), AvgInt);

	//float AvgFloat = Avg<float>(3.45f, 7.86f);
	//UE_LOG(LogTemp, Warning, TEXT("Avg of 3.45 and 7.86: %f"), AvgFloat);

#pragma endregion 

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);	// 콜백 함수를 델리게이트에 연결하는 매크로, 인텔리센스에서 자동완성되지 않으므로 주의
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);	// 이벤트 이름.AddDynamic(this, 연결할 콜백 함수 이름) 식으로 작성
}

float AItem::TransformedSin(float Value)
{
	return Amplitude * FMath::Sin(Value * TimeConstant);
}

float AItem::TransformedCos(float Value)
{
	return Amplitude * FMath::Cos(Value * TimeConstant);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);

	if (SlashCharacter)
	{
		SlashCharacter->SetOverlappingItem(this);	// 플레이어 캐릭터가 충돌하면 플레이어에 있는 아이템 포인터가 이 아이템을 가리키도록 설정
	}

	//const FString OtherActorName = OtherActor->GetName();

	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(1, 30.f, FColor::Red, OtherActorName);
	//}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const FString OtherActorName = FString("Ending Overlap with: ") + OtherActor->GetName();	// FString은 + 연산 지원

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 30.f, FColor::Red, OtherActorName);
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (GEngine)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("DeltaTime: %f"), DeltaTime);

	//	FString Name = GetName();
	//	FString Message = FString::Printf(TEXT("Item Name: %s"), *Name);
	//	GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Cyan, Message);

	//	UE_LOG(LogTemp, Warning, TEXT("Item Name: %s"), *Name);
	//}

	RunningTime += DeltaTime;
	//float DeltaZ = Amplitude * FMath::Sin(RunningTime * TimeConstant);

	//AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));

	//FVector Location = GetActorLocation();
	//DRAW_SPHERE_SINGLEFRAME(Location);
	//DRAW_VECTOR_SINGLEFRAME(Location, Location + GetActorForwardVector() * 100.f);

	//FVector AvgVector = Avg<FVector>(GetActorLocation(), FVector::ZeroVector);
	//DRAW_POINT_SINGLEFRAME(AvgVector);

	//FRotator AvgRotator = Avg<FRotator>(GetActorRotation(), FRotator::ZeroRotator);	//ERROR, FRotator에는 나누기 연산이 없음

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin(RunningTime)));
	}
}

