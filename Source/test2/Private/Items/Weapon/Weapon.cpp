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
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);	// 콜리전 설정을 Query만 검출하도록 설정
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);		// 모든 채널과의 콜리전 정책을 오버랩으로 바꾸는 함수 SetCollisionResponseTo"AllChannels"
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);		// Pawn과의 콜리전 정책을 Ignore로 바꿈	SetCollisionResponseTo"Channel"

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);	// 델리게이트 함수에 우리가 만든 OnBoxOverlap을 연결하는 매크로
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
	AttachMeshToSocket(InParent, InSocketName);		// 지정한 메시의 지정한 소켓에 해당 메시를 붙이는 함수
	ItemState = EItemState::EIS_Equipped;
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(	// 지정한 위치에서 들리는 소리를 구현해서 재생하는 함수
			this,	// 스태틱 함수라 월드에 대해서 알아야 하는데, 호출한 객체 본인을 넘겨도 월드 정보를 넘겨주는것과 비슷해서 this로 넘김
			EquipSound,	// 재생할 사운드
			GetActorLocation()	//재생할 위치
		);
	}

	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);	// 장착 후 아이템의 콜리전 구체를 꺼서 콜리전 구체와 플레이어 간 상호작용이 일어나지 않도록 함
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
	const FVector Start = BoxTraceStart->GetComponentLocation();	// BoxTraceStart의 위치를 가져오는 함수
	const FVector End = BoxTraceEnd->GetComponentLocation();		// BoxTraceEnd의 위치를 가저오는 함수

	TArray<AActor*> ActorsToIgnore;	// TArray: 언리얼5의 동적 배열
	ActorsToIgnore.Add(this);

	FHitResult BoxHit;

	UKismetSystemLibrary::BoxTraceSingle(
		this,	// 월드 컨텍스트 오브젝트
		Start,	// 시작 지점
		End,	// 끝 지점
		FVector(5.f, 5.f, 5.f),	// 트레이스에 사용할 박스의 크기
		BoxTraceStart->GetComponentRotation(),	// 트레이스에 사용할 박스의 회전값
		ETraceTypeQuery::TraceTypeQuery1,	// 트레이스에서 확인할 채널
		false,	// bTraceComplex: 간단한 충돌이 아닌 실제 메시와 충돌검사를 할 것인지 설정하는 매개변수
		ActorsToIgnore,		// 무시할 액터들을 담아놓은 동적 배열 전달
		EDrawDebugTrace::ForDuration,	// 충돌이 검출되었을 때 디버그 드로우를 어떻게 할지 결정
		BoxHit,		// 검출된 충돌에 대한 정보를 내보내는 출력용 매개변수
		true		// 충돌을 검출할 때 자기 자신을 생략할지 정하는 변수
	);
}
