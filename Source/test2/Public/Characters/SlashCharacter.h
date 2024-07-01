// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "SlashCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UGroomComponent;
class UAnimMontage;
class AItem;

UCLASS()
class TEST2_API ASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }	// FORCEINLINE: 강제로 함수를 인라인으로 만드는 매크로
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

protected:
	virtual void BeginPlay() override;
	
	// 입력 콜백 함수들
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
	virtual void Attack() override;

	// 전투 관련 함수들
	void EquipWeapon(AWeapon* Weapon);
	virtual void AttackEnd() override;	// 공격이 끝나는 시점에 플레이어의 ActionState를 EAS_Unoccupied로 다시 되돌리는 콜백 함수
	virtual bool CanAttack() override;
	bool CanDisarm();
	bool CanArm();
	void Disarm();
	void Arm();

	// 몽타주 관련 함수들
	void PlayEquipMontage(const FName& SectionName);	// 무기 집어넣기/꺼내기 몽타주 재생 함수


	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();	// 무기 집어넣기 함수

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();		// 무기 꺼내기 함수

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();	// 무기 집어넣기/꺼내기가 끝나는 시점에 플레이어의 ActionState를 EAS_Unoccupied로 다시 되돌리는 콜백 함수

private:
	// 컴포넌트
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

	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;
};
