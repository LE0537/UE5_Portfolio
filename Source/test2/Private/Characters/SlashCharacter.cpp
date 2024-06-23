// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASlashCharacter::ASlashCharacter()
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
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASlashCharacter::MoveForward(float Value)
{
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

// Called every frame
void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName(TEXT("MoveForward")), this, &ASlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName(TEXT("Turn")), this, &ASlashCharacter::Turn);
	PlayerInputComponent->BindAxis(FName(TEXT("LookUp")), this, &ASlashCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName(TEXT("MoveRight")), this, &ASlashCharacter::MoveRight);
}

