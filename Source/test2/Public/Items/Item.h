// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;

enum class EItemState : uint8	// 물체가 둥둥 떠다니는 처리를 할지 말지를 나타내는 enum class
{
	EIS_Hovering,	// 아이템이 떠다님
	EIS_Equipped	// 플레이어가 아이템을 장착한 상태라 떠다니지 않음
};

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

	UFUNCTION()	// UFUNCTION을 붙여야 제대로 델리게이트가 작동함
				// OnSphereOverlap은 Sphere를 가지고 충돌을 검사하는데, 이 Sphere를 포함한 원시적인 도형 컴포넌트들이 사용할 수 있는 PrimitiveComponent.h에서 
				// 이벤트들의 이름이 들어간 델리게이트 매크로를 찾아볼 수 있다. 이 중 우리가 사용하고 싶은 OnComponentBeginOverlap 이벤트를 가진 델리게이트를 확인하고
				// 이벤트 이름 옆에 있는 매개변수의 타입 이름들과 변수 이름들을 참고하여 우리가 사용할 함수(OnSphereOverlap)의 매개변수를 만든다.
				// OnComponentBeginOverlap은 6개의 매개변수가 필요한 델리게이트로, 정해진 6개의 매개변수를 우리 함수에 적용한다.
				// 이후 BeginPlay에서 AddDynamic 매크로를 이용하여 우리가 만든 콜백 함수를 델리게이트에 연결한다.
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()	// OnSphereEndOverlap은 함수가 더 이상 오버랩되지 않는 순간에 적용되는 이벤트이며, 이 이벤트는 PrimitiveComponent.h에서
				// 매개변수 4개를 쓰는 델리게이트 매크로에 연결되어 있다. 따라서 해당 매크로에 적혀있는 매개변수 타입들과 이름들을 참고하여 함수를 작성하고
				// 이후 BeginPlay에서 AddDynamic 매크로를 이용하여 우리가 만든 콜백 함수를 델리게이트에 연결한다.
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	EItemState ItemState;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;	// 충돌(오버랩)을 검출할 때 사용할 구체 컴포넌트

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
};

template<typename T>
inline T AItem::Avg(T First, T Second)
{
	return (First + Second) / 2;
}
