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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sine Parameter")	//BlueprintReadOnly : �������Ʈ�� Getter ��� ����
																				//BlueprintReadWrite : �������Ʈ�� Getter, Setter ��� ����
	float Amplitude;															//private ���������� �۵����� �����Ƿ� protected�� public�̿��� ��

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameter")	//Category : �������Ʈ �������� Detail�� ī�װ��� �߰��ϰų� ���� ī�װ��� ����
	float TimeConstant;

	//UFUNCTION(BlueprintCallable);	//�Լ��� �������Ʈ�� �����ϴ� ��ũ��
									//BlueprintCallable : �Լ��� �������Ʈ�� ������
	UFUNCTION(BlueprintPure)		//BlueprintPure : �Լ��� �������Ʈ�� �����Լ��� ������
	float TransformedSin(float Value);

	UFUNCTION(BlueprintPure)
	float TransformedCos(float Value);

	template<typename T>
	T Avg(T First, T Second);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//EditDefalutsOnly : ����Ʈ �������Ʈ������ ���� ������ �� ������ ���忡 �ִ� �ν��Ͻ������� �Ұ�����
	//EditInstanceOnly : ���忡 �ִ� �ν��Ͻ������� ���� ������ �� ������ ����Ʈ �������Ʈ������ �Ұ�����
	//EditAnywhere : ����Ʈ �������Ʈ�� ���忡 �ִ� �ν��Ͻ� �� �ٿ��� ���ų� ������ �� ����
	//VisibleDefaultOnly : ����Ʈ �������Ʈ���� �� ���� ������ ������ �Ұ�����
	//VisibleInstanceOnly : ���忡 �ִ� �ν��Ͻ������� �� �� ������ ������ �Ұ�����
	//VisibleAnywhere : ����Ʈ �������Ʈ�� ���忡 �ִ� �ν��Ͻ� �� �ٿ��� �� �� ������ ������ �Ұ�����

	//UPROPERTY(EditAnywhere);
	//UPROPERTY(VisibleInstanceOnly);
	//float RunningTime;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))	//<- private ������ �������Ʈ�� ���̰� �ϴ� ����
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
