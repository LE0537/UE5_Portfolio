// Fill out your copyright notice in the Description page of Project Settings.

// http://docs.unrealengine.com <- �𸮾� ���� ���� ���۷��� ���� 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
//#include "Components/CapsuleComponent.h"	// ĸ�� ������Ʈ ���, �𸮾� ���۷��� �������� ��� ����� ������� �˻��� �� ����
#include "Bird.generated.h"					// ��� ������� *.generated.h ���� �־�� ��

class UCapsuleComponent;					// ĸ�� ������Ʈ ���漱��
class USkeletalMeshComponent;				// ���̷��� �޽� ���漱��

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
