// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "SlashCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UGroomComponent;
class UAnimMontage;
class AItem;
class AWeapon;

UCLASS()
class TEST2_API ASlashCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASlashCharacter();

protected:
	// 입력 콜백 함수들
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
	void Attack();

	// 몽타주 재생 함수들
	void PlayAttackMontage();	// 무기 공격 몽타주 재생 함수
	UFUNCTION(BlueprintCallable)
	void AttackEnd();	// 공격이 끝나는 시점에 플레이어의 ActionState를 EAS_Unoccupied로 다시 되돌리는 콜백 함수
	bool CanAttack();

	void PlayEquipMontage(const FName& SectionName);	// 무기 집어넣기/꺼내기 몽타주 재생 함수
	bool CanDisarm();
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	void Disarm();	// 무기 집어넣기 함수

	UFUNCTION(BlueprintCallable)
	void Arm();		// 무기 꺼내기 함수

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();	// 무기 집어넣기/꺼내기가 끝나는 시점에 플레이어의 ActionState를 EAS_Unoccupied로 다시 되돌리는 콜백 함수

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);	// enum값을 매개변수로 넘기려면 이렇게 넘겨야 함

private:
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = "Hair")
	UGroomComponent* Hair;

	UPROPERTY(EditAnywhere, Category = "Hair")
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;

	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState;

	// 애니메이션 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;

public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }	// FORCEINLINE: 강제로 함수를 인라인으로 만드는 매크로
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};
