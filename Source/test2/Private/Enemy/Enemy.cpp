// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "DebugMacro.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Characters/CharacterTypes.h"

// Sets default values
AEnemy::AEnemy() : CombatTarget(nullptr), CombatRadius(500.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemy::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);

		FName SectionName = FName();
		
		const int32 Selection = FMath::RandRange(0, 3);
		switch (Selection)
		{
		case 0:
			SectionName = FName(TEXT("Death1"));
			DeathPose = EDeathPose::EDP_Death1;
			break;

		case 1:
			SectionName = FName(TEXT("Death2"));
			DeathPose = EDeathPose::EDP_Death2;
			break;

		case 2:
			SectionName = FName(TEXT("Death3"));
			DeathPose = EDeathPose::EDP_Death3;
			break;

		case 3:
			SectionName = FName(TEXT("Death4"));
			DeathPose = EDeathPose::EDP_Death4;
			break;

		default:
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(3.f);
	HealthBarWidget->SetVisibility(false);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CombatTarget)
	{
		const double DistanceToTarget = (CombatTarget->GetActorLocation() - GetActorLocation()).Size();
		if (DistanceToTarget > CombatRadius)
		{
			CombatTarget = nullptr;
			if (HealthBarWidget)
			{
				HealthBarWidget->SetVisibility(false);
			}
		}
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	DRAW_SPHERE_2S(ImpactPoint);

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}

	if (Attributes && Attributes->IsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		Die();
	}
	
	// 타격 사운드 재생
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(	// 타격 지점에서 사운드 플레이
			this,
			HitSound,
			ImpactPoint
		);
	}

	// 타격 파티클 생성
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(	// 타격 지점에서 파티클 생성
			GetWorld(),
			HitParticles,
			ImpactPoint
		);
	}
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// 전방벡터 * 타격 벡터 = 전방 벡터 길이 * 타격 벡터 길이 * 끼인 각 코사인
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	// 코사인 역함수(아크코사인)으로 각도 구하기
	double Theta = FMath::Acos(CosTheta);
	// 라디안을 일반 각도로 변환
	Theta = FMath::RadiansToDegrees(Theta);

	// 외적이 아래를 가리킨다면, 세타값은 음수가 되어야 한다
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0.f)
	{
		Theta *= -1.f;
	}

	FName Section(TEXT("FromBack"));

	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName(TEXT("FromFront"));
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName(TEXT("FromLeft"));
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName(TEXT("FromRight"));
	}

	PlayHitReactMontage(Section);

	//// 테스트; 맞은 각도를 화면에 디버그 메세지로 출력할 것임
	//if (GEngine)
	//{
	//	FString Message = FString::Printf(TEXT("Theta: %f"), Theta);
	//	GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Green, Message);
	//}

	//// 디버그 화살표 그리기
	//// 적의 포워드 벡터
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 120.f, 5.f, FColor::Red, 5.f);
	//// 타격 벡터
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 120.f, 5.f, FColor::Blue, 5.f);
	//// 외적 법선 벡터
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 120.f, 5.f, FColor::Green, 5.f);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);

		if (HealthBarWidget)
		{
			HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
		}
	}

	CombatTarget = EventInstigator->GetPawn();

	return DamageAmount;
}

// AActor::TakeDamage
// float DamageAmount: 대미지 수치
// FDamageEvent& DamageEvent: 대미지에 관한 추가적인 정보
// AController* Instigator: 대미지를 입힌 컨트롤러
// AActor* DamageCauser: 직접적으로 대미지를 입힌 액터

// UGameplayStatics::ApplyDamage
// DamageTyepClass: 대미지 타입을 정해서 사용자가 커스텀으로 대미지타입에 따른 처리를 할 수 있도록 함

// 만약 플레이어가 적에게 대미지를 입힌다면 UGameplayStatics::ApplyDamage()를 호출하면서 매개변수로 적을 넘김
// 대미지를 받은 적은 ApplyDamage에서 넘겨받은 정보를 바탕으로 AActor::TakeDamage를 호출하여 대미지에 대한 처리를 함


