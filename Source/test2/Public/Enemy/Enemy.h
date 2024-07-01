// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/CharacterTypes.h"
#include "Characters/BaseCharacter.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UPawnSensingComponent;
class AAIController;
class AWeapon;

UCLASS()
class TEST2_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	// Actor
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	// ~Actor

	// HitInterface
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	// HitInterface

protected:
	// Actor
	virtual void BeginPlay() override;
	// ~Actor

	// BaseCharacter
	virtual void Die() override;
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual int32 PlayDeathMontage() override;
	virtual void AttackEnd() override;
	// ~BaseCharacter

	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState;

private:
	/*
	*	컴포넌트
	*/

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;

	/*
	*	전투
	*/

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	float CombatRadius;

	UPROPERTY(EditAnywhere)
	float AttackRadius;

	void StartAttackTimer();
	void ClearAttackTimer();

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 8.f;

	/*
	*	AI 네비게이션
	*/

	UPROPERTY()
	AAIController* EnemyController;

	// 현재 순찰 목표
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	// 순찰 목표 모음
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	double PatrolRadius;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;							// Ctrl + R -> Ctrl + R 누르면 솔루션 내에 있는 해당 이름의 변수명을 한 번에 다 바꿀 수 있음

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;

	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void ClearPatrolTimer();
	void PatrolTimerFinished();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void SpawnDefaultWeapon();
	void InitializeEnemy();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);		// 폰 감지 컴포넌트의 OnPawnSeen(폰을 감지했을 때) 이벤트를 위한 콜백 함수
};
