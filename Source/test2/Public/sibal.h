// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "sibal.generated.h"

UCLASS()
class TEST2_API Asibal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Asibal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
