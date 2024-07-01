// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapon/Weapon.h"
#include "Animation/AnimMontage.h"

ASlashCharacter::ASlashCharacter() : CharacterState(ECharacterState::ECS_Unequipped), ActionState(EActionState::EAS_Unoccupied)
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationYaw = false;		// �÷��̾� ���� Yaw �������� ȸ���ϴ� ���� �����ϴ� ����
	bUseControllerRotationPitch = false;	// �÷��̾� ���� Pitch �������� ȸ���ϴ� ���� �����ϴ� ����
	bUseControllerRotationRoll = false;		// �÷��̾� ���� Roll �������� ȸ���ϴ� ���� �����ϴ� ����

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.f, 400.f, 0.f);

	// �޽� �ݶ��̴� ����
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

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

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (GEngine)
	//{
	//	FString message = FString::Printf(TEXT("ActionState: %d"), ActionState) ;
	//	GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Red, message);
	//}
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

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint)
{
	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Tags.Add(FName(TEXT("EngageableTarget")));	// ĳ��Ʈ�� ���̱� ���� �±� ����, Enemy PawnSeen ���� 
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
		EquipWeapon(OverlappingWeapon);
	}
	else	// �ݸ��� ��ü�� ��ȣ�ۿ����� �ʰ� ���� �� E�� ���� ���
	{
		if (CanDisarm())	// ���⸦ �������
		{
			Disarm();
		}

		else if (CanArm())	// ���⸦ ����
		{
			Arm();
		}
	}
}

void ASlashCharacter::Attack()
{
	Super::Attack();

	if(CanAttack())	
	{
		PlayAttackMontage();	// ���� �ִϸ��̼� ��Ÿ�� ���
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName(TEXT("RightHandSocket")), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;	// ���⸦ ������ �� �÷��̾ �� �̻� ����� ��ġ�� �ʴ´ٴ� ó���� �ϱ� ���� nullptr ó��
	EquippedWeapon = Weapon;	// ������ ����� EquippedWeapon���� ����Ű�� �����͸� ����
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
	PlayEquipMontage(FName(TEXT("Unequip")));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::Arm()
{
	PlayEquipMontage(FName(TEXT("Equip")));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
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

void ASlashCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName(TEXT("SpineSocket")));
	}
}

void ASlashCharacter::AttachWeaponToHand()
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