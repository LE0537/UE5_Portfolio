// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);	// �ݸ��� ������ Query�� �����ϵ��� ����
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);		// ��� ä�ΰ��� �ݸ��� ��å�� ���������� �ٲٴ� �Լ� SetCollisionResponseTo"AllChannels"
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);		// Pawn���� �ݸ��� ��å�� Ignore�� �ٲ�	SetCollisionResponseTo"Channel"

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);	// ��������Ʈ �Լ��� �츮�� ���� OnBoxOverlap�� �����ϴ� ��ũ��
}

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

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();	// BoxTraceStart�� ��ġ�� �������� �Լ�
	const FVector End = BoxTraceEnd->GetComponentLocation();		// BoxTraceEnd�� ��ġ�� �������� �Լ�

	TArray<AActor*> ActorsToIgnore;	// TArray: �𸮾�5�� ���� �迭
	ActorsToIgnore.Add(this);

	FHitResult BoxHit;

	UKismetSystemLibrary::BoxTraceSingle(
		this,	// ���� ���ؽ�Ʈ ������Ʈ
		Start,	// ���� ����
		End,	// �� ����
		FVector(5.f, 5.f, 5.f),	// Ʈ���̽��� ����� �ڽ��� ũ��
		BoxTraceStart->GetComponentRotation(),	// Ʈ���̽��� ����� �ڽ��� ȸ����
		ETraceTypeQuery::TraceTypeQuery1,	// Ʈ���̽����� Ȯ���� ä��
		false,	// bTraceComplex: ������ �浹�� �ƴ� ���� �޽ÿ� �浹�˻縦 �� ������ �����ϴ� �Ű�����
		ActorsToIgnore,		// ������ ���͵��� ��Ƴ��� ���� �迭 ����
		EDrawDebugTrace::ForDuration,	// �浹�� ����Ǿ��� �� ����� ��ο츦 ��� ���� ����
		BoxHit,		// ����� �浹�� ���� ������ �������� ��¿� �Ű�����
		true		// �浹�� ������ �� �ڱ� �ڽ��� �������� ���ϴ� ����
	);
}
