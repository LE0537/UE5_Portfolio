// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TEST2_API IHitInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 가상함수는 아니지만 오버라이드할 수 있음
	// 언리얼 엔진이 우리가 오버라이드할 수 있는 가상함수를 생성하지만, 이 함수 자체는 블루프린트에서 구현이 가능하도록 한다
	// 언리얼에서 만들어주는 C++ 전용 가상함수를 쓰려면 함수 이름 뒤에 _Implementation이 붙어야 한다.
	UFUNCTION(BlueprintNativeEvent)
	void GetHit(const FVector& ImpactPoint);
};
