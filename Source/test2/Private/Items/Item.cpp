// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "test2/DebugMacro.h"


// Sets default values
AItem::AItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

	DRAW_SPHERE(Location);
	//DRAW_LINE(Location, Location + Forward * 100.f);
	//DRAW_POINT(Location + Forward * 100.f);
	DRAW_VECTOR(Location, Location + Forward * 100.f);

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
}

