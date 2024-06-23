// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "test2/DebugMacro.h"

//Class Default Object (CDO) : �𸮾� ���� ���÷��� �ý��ۿ� ���� �����Ǵ� ��ü�� ���纻,
// ������ ���۵� ���� ������ �� ������, ���÷��� �ý����� �츮 Ŭ������ ����� �������Ʈ ��ü���� �ʱ�ȭ�� �� �ʿ��� ������ �����ϱ� ���� ������
// �׷��� ��ü�� �����ڿ��� �Լ� ���� �� �� ������ ��� ����� Ȱ��ȭ���� �ʾ��� �� �����Ƿ� �����ؾ� ��
// ���� �����ڰ� �ƴ� BeginPlay ��� ���� �� ó���� �͵��� �����ؾ� ��

//������Ʈ���� ����� ������ �Ͼ. ������Ʈ�� �����ϱ� ���ؼ� ����Ʈ ������ü�� ������ ��
// ����Ʈ ������ü�� CDO�� ��������� ������Ʈ�� ����Ʈ ������ ������ ����
// ������ü�� ���� ���� �� ���� ������ �����ؾ� ��
// 
//		1. ��ü Ÿ��(UStaticMeshComponent ��)
//		2. ���� �̸�(������Ʈ ���� �̸��� �ٸ�)
//		CreateDefaultSubobject<Type>(Name);
//		
//		�� �Լ��� ���� ������ ��ü�� ����Ű�� �����͸� ��ȯ�Ѵ�. �̷� �Լ��� ���丮��� �Ѵ�.
//		�𸮾� ���������� ���丮 �Լ��� ���� ����ϱ� ������ new�� ���� ������� �ʴ´�

// Sets default values
AItem::AItem() : Amplitude(0.25f), TimeConstant(5.f), RunningTime(0.f)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));	//����Ʈ ������ü ���� ���丮 �Լ�
	RootComponent = ItemMesh;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Begin Play Called!"));

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 60.f, FColor::Cyan, FString("Item OnScreen Message!"));
	}

	UWorld* World = GetWorld();
	FVector Location = GetActorLocation();
	FVector Forward = GetActorForwardVector();

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
}

float AItem::TransformedSin(float Value)
{
	return Amplitude * FMath::Sin(Value * TimeConstant);
}

float AItem::TransformedCos(float Value)
{
	return Amplitude * FMath::Cos(Value * TimeConstant);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GEngine)
	{
		UE_LOG(LogTemp, Warning, TEXT("DeltaTime: %f"), DeltaTime);

		FString Name = GetName();
		FString Message = FString::Printf(TEXT("Item Name: %s"), *Name);
		GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Cyan, Message);

		UE_LOG(LogTemp, Warning, TEXT("Item Name: %s"), *Name);
	}

	RunningTime += DeltaTime;
	//float DeltaZ = Amplitude * FMath::Sin(RunningTime * TimeConstant);

	//AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));

	//FVector Location = GetActorLocation();
	//DRAW_SPHERE_SINGLEFRAME(Location);
	//DRAW_VECTOR_SINGLEFRAME(Location, Location + GetActorForwardVector() * 100.f);

	//FVector AvgVector = Avg<FVector>(GetActorLocation(), FVector::ZeroVector);
	//DRAW_POINT_SINGLEFRAME(AvgVector);

	//FRotator AvgRotator = Avg<FRotator>(GetActorRotation(), FRotator::ZeroRotator);	//ERROR, FRotator���� ������ ������ ����



}

