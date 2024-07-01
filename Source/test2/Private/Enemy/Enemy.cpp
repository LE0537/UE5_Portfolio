// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "DebugMacro.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Characters/CharacterTypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
AEnemy::AEnemy() : CombatTarget(nullptr), CombatRadius(500.f), PatrolRadius(200.f), EnemyState(EEnemyState::EES_Patrolling), AttackRadius(150.f)
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

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

	GetCharacterMovement()->MaxWalkSpeed = 125.f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}

	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}
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

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (!Target) return false;

	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();

	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (!EnemyController || !Target)
		return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(15.f);

	EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (auto Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}

	return nullptr;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (EnemyState == EEnemyState::EES_Chasing)
	{
		return;
	}

	if (SeenPawn->ActorHasTag(FName(TEXT("SlashCharacter"))))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);	// �߰��� �����ϱ� ������ Ÿ�̸� �ʱ�ȭ
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		CombatTarget = SeenPawn;
		
		if (EnemyState != EEnemyState::EES_Attacking)
		{
			EnemyState = EEnemyState::EES_Chasing;
			MoveToTarget(CombatTarget);
		}
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ( EnemyState > EEnemyState::EES_Patrolling )
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
		// Ÿ�̸Ӹ� ������, �Ű������� Ÿ�̸� ����, �Լ��� �ִ� ��ü, Ÿ�̸Ӱ� �� �Ǿ��� �� ����� �Լ�, �ð��� ����
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		// ���� ���� ������ ������ ��׷ΰ� Ǯ������ ����
		CombatTarget = nullptr;
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(false);
		}

		EnemyState = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = 125.f;
		MoveToTarget(PatrolTarget);
		UE_LOG(LogTemp, Warning, TEXT("Lost Interest"));
	}

	else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
	{
		// ���� ���� ���̸� ���ΰ��� ������
		EnemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		MoveToTarget(CombatTarget);
		UE_LOG(LogTemp, Warning, TEXT("Chase Player"));
	}

	else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking)
	{
		// ���� ���� ���̸� ���ΰ��� ������
		EnemyState = EEnemyState::EES_Attacking;
		// TODO: ���� ��Ÿ�� ����
		UE_LOG(LogTemp, Warning, TEXT("Attack Player"));
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
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
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	MoveToTarget(CombatTarget);
	return DamageAmount;
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
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


