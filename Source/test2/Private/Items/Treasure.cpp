// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);

	if (SlashCharacter)
	{
		// 아이템 획득 시 플레이어에게 전달

		// 아이템 획득 지점에서 사운드 플레이
		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(	
				this,
				PickupSound,
				GetActorLocation()
			);
		}

		// 획득한 아이템을 파괴
		Destroy();
	}

	//const FString OtherActorName = OtherActor->GetName();

	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(1, 30.f, FColor::Red, OtherActorName);
	//}
}