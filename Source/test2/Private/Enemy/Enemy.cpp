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
	
	// Ÿ�� ���� ���
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(	// Ÿ�� �������� ���� �÷���
			this,
			HitSound,
			ImpactPoint
		);
	}

	// Ÿ�� ��ƼŬ ����
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(	// Ÿ�� �������� ��ƼŬ ����
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

	// ���溤�� * Ÿ�� ���� = ���� ���� ���� * Ÿ�� ���� ���� * ���� �� �ڻ���
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	// �ڻ��� ���Լ�(��ũ�ڻ���)���� ���� ���ϱ�
	double Theta = FMath::Acos(CosTheta);
	// ������ �Ϲ� ������ ��ȯ
	Theta = FMath::RadiansToDegrees(Theta);

	// ������ �Ʒ��� ����Ų�ٸ�, ��Ÿ���� ������ �Ǿ�� �Ѵ�
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

	//// �׽�Ʈ; ���� ������ ȭ�鿡 ����� �޼����� ����� ����
	//if (GEngine)
	//{
	//	FString Message = FString::Printf(TEXT("Theta: %f"), Theta);
	//	GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Green, Message);
	//}

	//// ����� ȭ��ǥ �׸���
	//// ���� ������ ����
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 120.f, 5.f, FColor::Red, 5.f);
	//// Ÿ�� ����
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 120.f, 5.f, FColor::Blue, 5.f);
	//// ���� ���� ����
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
// float DamageAmount: ����� ��ġ
// FDamageEvent& DamageEvent: ������� ���� �߰����� ����
// AController* Instigator: ������� ���� ��Ʈ�ѷ�
// AActor* DamageCauser: ���������� ������� ���� ����

// UGameplayStatics::ApplyDamage
// DamageTyepClass: ����� Ÿ���� ���ؼ� ����ڰ� Ŀ�������� �����Ÿ�Կ� ���� ó���� �� �� �ֵ��� ��

// ���� �÷��̾ ������ ������� �����ٸ� UGameplayStatics::ApplyDamage()�� ȣ���ϸ鼭 �Ű������� ���� �ѱ�
// ������� ���� ���� ApplyDamage���� �Ѱܹ��� ������ �������� AActor::TakeDamage�� ȣ���Ͽ� ������� ���� ó���� ��


