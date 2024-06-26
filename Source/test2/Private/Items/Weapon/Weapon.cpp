// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
	AttachMeshToSocket(InParent, InSocketName);		// ������ �޽��� ������ ���Ͽ� �ش� �޽ø� ���̴� �Լ�
	ItemState = EItemState::EIS_Equipped;
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(	// ������ ��ġ���� �鸮�� �Ҹ��� �����ؼ� ����ϴ� �Լ�
			this,	// ����ƽ �Լ��� ���忡 ���ؼ� �˾ƾ� �ϴµ�, ȣ���� ��ü ������ �Ѱܵ� ���� ������ �Ѱ��ִ°Ͱ� ����ؼ� this�� �ѱ�
			EquipSound,	// ����� ����
			GetActorLocation()	//����� ��ġ
		);
	}

	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);	// ���� �� �������� �ݸ��� ��ü�� ���� �ݸ��� ��ü�� �÷��̾� �� ��ȣ�ۿ��� �Ͼ�� �ʵ��� ��
	}
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
