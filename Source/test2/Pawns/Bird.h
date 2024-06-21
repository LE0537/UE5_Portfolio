// Fill out your copyright notice in the Description page of Project Settings.

// http://docs.unrealengine.com <- 언리얼 엔진 공식 레퍼런스 문서 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
//#include "Components/CapsuleComponent.h"	// 캡슐 컴포넌트 헤더, 언리얼 레퍼런스 문서에서 어떻게 헤더를 써야할지 검색할 수 있음
#include "Bird.generated.h"					// 모든 헤더들은 *.generated.h 위에 넣어야 함

class UCapsuleComponent;					// 캡슐 컴포넌트 전방선언
class USkeletalMeshComponent;				// 스켈레톤 메시 전방선언

UCLASS()
class TEST2_API ABird : public APawn
{
	GENERATED_BODY()

public:
	ABird();
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* BirdMesh;
};
