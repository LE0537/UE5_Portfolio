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

	bUseControllerRotationYaw = false;		// 플레이어 모델이 Yaw 방향으로 회전하는 것을 결정하는 변수
	bUseControllerRotationPitch = false;	// 플레이어 모델이 Pitch 방향으로 회전하는 것을 결정하는 변수
	bUseControllerRotationRoll = false;		// 플레이어 모델이 Roll 방향으로 회전하는 것을 결정하는 변수

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.f, 400.f, 0.f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;	// 폰의 컨트롤러의 회전을 사용하여 스프링암을 회전시킬지 결정하는 변수

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh(), FName(TEXT("head")));		// 버그인지 뭔지 작동이 안돼서 소켓 강제지정
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh(), FName(TEXT("head")));	// 버그인지 뭔지 작동이 안돼서 소켓 강제지정
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

		const FRotator ControlRotation = GetControlRotation();		// 컨트롤러의 회전값
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);	// 컨트롤러의 회전값에서 Yaw값만 따온 것

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);	// YawRotation을 기준으로 한 회전행렬에서 Look 방향의 단위벡터를 가져옴
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

		const FRotator ControlRotation = GetControlRotation();		// 컨트롤러의 회전값
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);	// 컨트롤러의 회전값에서 Yaw값만 따온 것

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);	// YawRotation을 기준으로 한 회전행렬에서 Right 방향의 단위벡터를 가져옴
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

	if (OverlappingWeapon)	// 1. 월드에 놓여 있는 무기의 콜리전 구체와 플레이어가 겹쳤을 경우
	{
		OverlappingWeapon->Equip(GetMesh(), FName(TEXT("RightHandSocket")));
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		OverlappingItem = nullptr;	// 무기를 장착한 후 플레이어가 더 이상 무기와 겹치지 않는다는 처리를 하기 위해 nullptr 처리
		EquippedWeapon = OverlappingWeapon;	// 장착한 무기는 EquippedWeapon으로 가리키는 포인터를 변경
	}
	else	// 콜리전 구체와 상호작용하지 않고 있을 때 E를 누를 경우
	{
		if (CanDisarm())	// 무기를 집어넣음
		{
			PlayEquipMontage(FName(TEXT("Unequip")));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}

		else if (CanArm())	// 무기를 꺼냄
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
		PlayAttackMontage();	// 공격 애니메이션 몽타주 재생
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);	// 몽타주 재생 함수
		// 예시(강의 연습)
		const int32 Selection = FMath::RandRange(0, 2);
		FName SectionName = FName();

		// 실제 게임 프로그래밍 시에는 enum 값으로 순차적으로 공격 애니메이션이 재생되도록 설정할것임

		switch (Selection)	// 선택된 공격 인덱스에 따라 섹션 네임 설정
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

		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);	//선택된 섹션 네임으로 몽타주의 재생 시점 이동
	}
}

void ASlashCharacter::AttackEnd()	// 공격이 끝나는 시점에 플레이어의 ActionState를 EAS_Unoccupied로 다시 되돌리는 콜백 함수
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
		AnimInstance->Montage_Play(EquipMontage);	// 장착 애니메이션 몽타주 재생
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);	// 선택된 섹션 네임으로 몽타주의 재생 시점 이동
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

	// 액션 매핑 함수, IE_Pressed는 눌렀을 때 작동된다는 뜻
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

