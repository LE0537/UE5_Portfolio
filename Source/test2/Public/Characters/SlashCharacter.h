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
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }	// FORCEINLINE: ������ �Լ��� �ζ������� ����� ��ũ��
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

protected:
	virtual void BeginPlay() override;
	
	// �Է� �ݹ� �Լ���
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
	virtual void Attack() override;

	// ���� ���� �Լ���
	void EquipWeapon(AWeapon* Weapon);
	virtual void AttackEnd() override;	// ������ ������ ������ �÷��̾��� ActionState�� EAS_Unoccupied�� �ٽ� �ǵ����� �ݹ� �Լ�
	virtual bool CanAttack() override;
	bool CanDisarm();
	bool CanArm();
	void Disarm();
	void Arm();

	// ��Ÿ�� ���� �Լ���
	void PlayEquipMontage(const FName& SectionName);	// ���� ����ֱ�/������ ��Ÿ�� ��� �Լ�


	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();	// ���� ����ֱ� �Լ�

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();		// ���� ������ �Լ�

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();	// ���� ����ֱ�/�����Ⱑ ������ ������ �÷��̾��� ActionState�� EAS_Unoccupied�� �ٽ� �ǵ����� �ݹ� �Լ�

private:
	// ������Ʈ
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
