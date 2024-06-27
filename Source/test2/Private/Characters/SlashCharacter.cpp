// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapon/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"

ASlashCharacter::ASlashCharacter() : CharacterState(ECharacterState::ECS_Unequipped), ActionState(EActionState::EAS_Unoccupied)
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;		// �÷��̾� ���� Yaw �������� ȸ���ϴ� ���� �����ϴ� ����
	bUseControllerRotationPitch = false;	// �÷��̾� ���� Pitch �������� ȸ���ϴ� ���� �����ϴ� ����
	bUseControllerRotationRoll = false;		// �÷��̾� ���� Roll �������� ȸ���ϴ� ���� �����ϴ� ����

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.f, 400.f, 0.f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;	// ���� ��Ʈ�ѷ��� ȸ���� ����Ͽ� ���������� ȸ����ų�� �����ϴ� ����

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh(), FName(TEXT("head")));		// �������� ���� �۵��� �ȵż� ���� ��������
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh(), FName(TEXT("head")));	// �������� ���� �۵��� �ȵż� ���� ��������
	Eyebrows->AttachmentName = FString("head");
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASlashCharacter::MoveForward(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied)
	{
		return;
	}

	if (Controller && Value != 0.f)
	{
		//FVector Forward = GetActorForwardVector();
		//AddMovementInput(Forward, Value);

		const FRotator ControlRotation = GetControlRotation();		// ��Ʈ�ѷ��� ȸ����
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);	// ��Ʈ�ѷ��� ȸ�������� Yaw���� ���� ��

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);	// YawRotation�� �������� �� ȸ����Ŀ��� Look ������ �������͸� ������
		AddMovementInput(Direction, Value);
	}
}

void ASlashCharacter::MoveRight(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied)
	{
		return;
	}

	if (Controller && Value != 0.f)
	{
		//FVector Right = GetActorRightVector();
		//AddMovementInput(Right, Value);

		const FRotator ControlRotation = GetControlRotation();		// ��Ʈ�ѷ��� ȸ����
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);	// ��Ʈ�ѷ��� ȸ�������� Yaw���� ���� ��

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);	// YawRotation�� �������� �� ȸ����Ŀ��� Right ������ �������͸� ������
		AddMovementInput(Direction, Value);
	}
}

void ASlashCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ASlashCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ASlashCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);

	if (OverlappingWeapon)	// 1. ���忡 ���� �ִ� ������ �ݸ��� ��ü�� �÷��̾ ������ ���
	{
		OverlappingWeapon->Equip(GetMesh(), FName(TEXT("RightHandSocket")));
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		OverlappingItem = nullptr;	// ���⸦ ������ �� �÷��̾ �� �̻� ����� ��ġ�� �ʴ´ٴ� ó���� �ϱ� ���� nullptr ó��
		EquippedWeapon = OverlappingWeapon;	// ������ ����� EquippedWeapon���� ����Ű�� �����͸� ����
	}
	else	// �ݸ��� ��ü�� ��ȣ�ۿ����� �ʰ� ���� �� E�� ���� ���
	{
		if (CanDisarm())	// ���⸦ �������
		{
			PlayEquipMontage(FName(TEXT("Unequip")));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}

		else if (CanArm())	// ���⸦ ����
		{
			PlayEquipMontage(FName(TEXT("Equip")));
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
}

void ASlashCharacter::Attack()
{
	if(CanAttack())	
	{
		PlayAttackMontage();	// ���� �ִϸ��̼� ��Ÿ�� ���
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);	// ��Ÿ�� ��� �Լ�
		// ����(���� ����)
		const int32 Selection = FMath::RandRange(0, 2);
		FName SectionName = FName();

		// ���� ���� ���α׷��� �ÿ��� enum ������ ���������� ���� �ִϸ��̼��� ����ǵ��� �����Ұ���

		switch (Selection)	// ���õ� ���� �ε����� ���� ���� ���� ����
		{
		case 0:
			SectionName = FName(TEXT("Attack1"));
			break;
		case 1:
			SectionName = FName(TEXT("Attack2"));
			break;
		case 2:
			SectionName = FName(TEXT("Attack3"));
			break;
		default:
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);	//���õ� ���� �������� ��Ÿ���� ��� ���� �̵�
	}
}

void ASlashCharacter::AttackEnd()	// ������ ������ ������ �÷��̾��� ActionState�� EAS_Unoccupied�� �ٽ� �ǵ����� �ݹ� �Լ�
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);	// ���� �ִϸ��̼� ��Ÿ�� ���
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);	// ���õ� ���� �������� ��Ÿ���� ��� ���� �̵�
	}
}

bool ASlashCharacter::CanDisarm()
{
	return CharacterState != ECharacterState::ECS_Unequipped &&
		ActionState == EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

void ASlashCharacter::Disarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName(TEXT("SpineSocket")));
	}
}

void ASlashCharacter::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName(TEXT("RightHandSocket")));
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

// Called every frame
void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GEngine)
	{
		FString message = FString::Printf(TEXT("ActionState: %d"), ActionState) ;
		GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Red, message);
	}
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName(TEXT("MoveForward")), this, &ASlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName(TEXT("Turn")), this, &ASlashCharacter::Turn);
	PlayerInputComponent->BindAxis(FName(TEXT("LookUp")), this, &ASlashCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName(TEXT("MoveRight")), this, &ASlashCharacter::MoveRight);

	// �׼� ���� �Լ�, IE_Pressed�� ������ �� �۵��ȴٴ� ��
	PlayerInputComponent->BindAction(FName(TEXT("Jump")), IE_Pressed, this, &ACharacter::Jump);	
	PlayerInputComponent->BindAction(FName(TEXT("Equip")), IE_Pressed, this, &ASlashCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName(TEXT("Attack")), IE_Pressed, this, &ASlashCharacter::Attack);
}

void ASlashCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->ClearIgnoreActors();
	}
}

