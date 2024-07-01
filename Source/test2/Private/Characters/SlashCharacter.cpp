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

	bUseControllerRotationYaw = false;		// 플레이어 모델이 Yaw 방향으로 회전하는 것을 결정하는 변수
	bUseControllerRotationPitch = false;	// 플레이어 모델이 Pitch 방향으로 회전하는 것을 결정하는 변수
	bUseControllerRotationRoll = false;		// 플레이어 모델이 Roll 방향으로 회전하는 것을 결정하는 변수

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.f, 400.f, 0.f);

	// 메쉬 콜라이더 설정
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

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

	// 액션 매핑 함수, IE_Pressed는 눌렀을 때 작동된다는 뜻
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
	
	Tags.Add(FName(TEXT("EngageableTarget")));	// 캐스트를 줄이기 위한 태그 설정, Enemy PawnSeen 참고 
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
		EquipWeapon(OverlappingWeapon);
	}
	else	// 콜리전 구체와 상호작용하지 않고 있을 때 E를 누를 경우
	{
		if (CanDisarm())	// 무기를 집어넣음
		{
			Disarm();
		}

		else if (CanArm())	// 무기를 꺼냄
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
		PlayAttackMontage();	// 공격 애니메이션 몽타주 재생
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName(TEXT("RightHandSocket")), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;	// 무기를 장착한 후 플레이어가 더 이상 무기와 겹치지 않는다는 처리를 하기 위해 nullptr 처리
	EquippedWeapon = Weapon;	// 장착한 무기는 EquippedWeapon으로 가리키는 포인터를 변경
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
		AnimInstance->Montage_Play(EquipMontage);	// 장착 애니메이션 몽타주 재생
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);	// 선택된 섹션 네임으로 몽타주의 재생 시점 이동
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