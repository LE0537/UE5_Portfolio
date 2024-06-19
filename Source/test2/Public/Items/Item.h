// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class TEST2_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sine Parameter")	//BlueprintReadOnly : 블루프린트에 Getter 노드 생성
																				//BlueprintReadWrite : 블루프린트에 Getter, Setter 노드 생성
	float Amplitude;															//private 변수에서는 작동하지 않으므로 protected나 public이여야 함

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameter")	//Category : 블루프린트 오른쪽의 Detail에 카테고리를 추가하거나 기존 카테고리에 삽입
	float TimeConstant;

	//UFUNCTION(BlueprintCallable);	//함수를 블루프린트에 노출하는 매크로
									//BlueprintCallable : 함수를 블루프린트에 노출함
	UFUNCTION(BlueprintPure)		//BlueprintPure : 함수를 블루프린트에 순수함수로 노출함
	float TransformedSin(float Value);

	UFUNCTION(BlueprintPure)
	float TransformedCos(float Value);

	template<typename T>
	T Avg(T First, T Second);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//EditDefalutsOnly : 디폴트 블루프린트에서는 보고 수정할 수 있지만 월드에 있는 인스턴스에서는 불가능함
	//EditInstanceOnly : 월드에 있는 인스턴스에서는 보고 수정할 수 있지만 디폴트 블루프린트에서는 불가능함
	//EditAnywhere : 디폴트 블루프린트나 월드에 있는 인스턴스 둘 다에서 보거나 수정할 수 있음
	//VisibleDefaultOnly : 디폴트 블루프린트에서 볼 수는 있지만 수정은 불가능함
	//VisibleInstanceOnly : 월드에 있는 인스턴스에서는 볼 수 있지만 수정은 불가능함
	//VisibleAnywhere : 디폴트 블루프린트나 월드에 있는 인스턴스 둘 다에서 볼 수 있지만 수정은 불가능함

	//UPROPERTY(EditAnywhere);
	//UPROPERTY(VisibleInstanceOnly);
	//float RunningTime;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))	//<- private 변수도 블루프린트에 보이게 하는 문법
	float RunningTime;

	//UPROPERTY(EditAnywhere);
	//float Amplitude;

	//UPROPERTY(EditAnywhere);
	//float TimeConstant;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ItemMesh;
};

template<typename T>
inline T AItem::Avg(T First, T Second)
{
	return (First + Second) / 2;
}
