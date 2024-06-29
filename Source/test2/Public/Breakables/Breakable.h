// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "Breakable.generated.h"

class UGeometryCollectionComponent;
class UCapsuleComponent;
class ATreasure;

UCLASS()
class TEST2_API ABreakable : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABreakable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint);

	UFUNCTION()
	void OnChaosBreakEvent(const FChaosBreakEvent& BreakEvent);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* Capsule;

private:
	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollection;

	//UPROPERTY(EditAnywhere)
	//UClass* TreasureClass;	// 항아리를 깨면 나올 보물

	//UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	//TSubclassOf<ATreasure> TreasureClass;	// UClass 변수는 아무 클래스나 받을 수 있어 실수할 가능성이 있지만
											// TSubclassOf는 UClass를 반환하면서도 ATreasure의 파생클래스만 받아들임

	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	TArray<TSubclassOf<ATreasure>> TreasureClasses;		// 여러 오브젝트들 중에서 랜덤으로 나오게 할 것이므로 벡터(TArray)에 보물 블루프린트 클래스들을 저장

	bool bBroken;
};
