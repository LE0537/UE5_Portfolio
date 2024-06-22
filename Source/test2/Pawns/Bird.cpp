// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Pawns/Bird.h"

ABird::ABird()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));

	Capsule->SetCapsuleHalfHeight(20.f);	// 캡슐의 절반 높이를 설정하는 함수
	Capsule->SetCapsuleRadius(15.f);		// 캡슐의 반지름 길이를 설정하는 함수

	SetRootComponent(Capsule);	// == RootComponent = Capsule;

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(GetRootComponent());	// 새의 메시를 루트 컴포넌트에 붙이는(Attach) 함수, 이거 안 하면 루트 컴포넌트가 움직여도 메시는 제자리에 있음 

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));	// 만약 블루프린트에서 디테일 항목이 안 뜨면 변수 이름을 한 번 바꿔봐라
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 150.f;											// 스프링암의 길이

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	AutoPossessPlayer = EAutoReceiveInput::Player0;	// 플레이어가 해당 폰에게 자동으로 빙의(Possess)하도록 하는 변수
}

void ABird::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABird::MoveForward(float Value)	// 매 프레임마다 호출
{
	UE_LOG(LogTemp, Warning, TEXT("Value: %f"), Value);

	if (Controller && (Value != 0.f))
	{
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, Value);	// 컨트롤러에 있는 이동(Movement) 컴포넌트에 정보를 전달하는 함수
	}
}

void ABird::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABird::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName(TEXT("MoveForward")), this, &ABird::MoveForward);	// 함수를 인풋과 연결시켜주는 함수
																							// 프로젝트 설정에서 정한 축 입력 이름, 이 객체, 연결할 함수의 주소를 입력해야 함
	PlayerInputComponent->BindAxis(FName(TEXT("Turn")), this, &ABird::Turn);				// 마우스 좌우(yaw) 회전 함수 연결
	PlayerInputComponent->BindAxis(FName(TEXT("LookUp")), this, &ABird::LookUp);			// 마우스 상하(pitch) 회전 함수 연결
}

