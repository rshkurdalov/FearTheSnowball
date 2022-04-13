
#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "UI/EndGameWidget.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseCameraRotator = CreateDefaultSubobject<USceneComponent>(TEXT("BaseCameraRotator"));
	BaseCameraRotator->SetupAttachment(RootComponent);
	BaseCameraRotator->SetRelativeRotation(FRotator(345.f, 0.f, 0.f));
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent->SetupAttachment(BaseCameraRotator);
	CameraComponent->SetRelativeLocation(FVector(-350, 0, 0.f));
	CameraComponent->FieldOfView = 100.f;
}


void APlayerCharacter::EndGame_Implementation(bool isWinner)
{
	if (HasAuthority() && GetNetMode() == NM_DedicatedServer)
		return;

	EndGameWidget = CreateWidget<UEndGameWidget>(Cast<APlayerController>(GetController()), EndGameWidgetClass);
	
	if (!EndGameWidget)
		return;

	EndGameWidget->AddToViewport();
	EndGameWidget->OnEndGame(isWinner);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveBackward", this, &APlayerCharacter::MoveBackward);
	PlayerInputComponent->BindAxis("MoveLeft", this, &APlayerCharacter::MoveLeft);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookVertical", this, &APlayerCharacter::LookVertical);
	PlayerInputComponent->BindAxis("LookHorizontal", this, &APawn::AddControllerYawInput);
}

void APlayerCharacter::MoveForward(float Value) 
{
	const FVector delta = GetActorForwardVector() * Value;
	AddMovementInput(delta);
}

void APlayerCharacter::MoveBackward(float Value) 
{
	const FVector delta = -GetActorForwardVector() * Value;
	AddMovementInput(delta);
}

void APlayerCharacter::MoveLeft(float Value) 
{
	const FVector delta = -GetActorRightVector() * Value;
	AddMovementInput(delta);
}

void APlayerCharacter::MoveRight(float Value) 
{
	const FVector delta = GetActorRightVector() * Value;
	AddMovementInput(delta);
}

void APlayerCharacter::LookVertical(float Value) 
{
	const float futureResult = BaseCameraRotator->GetRelativeRotation().Pitch + Value;

	// restrict movement to [270;360]
	if (FMath::Sin(FMath::DegreesToRadians(futureResult)) > 0.01f || FMath::Cos(FMath::DegreesToRadians(futureResult)) < 0.01f)
		return;

	BaseCameraRotator->AddRelativeRotation(FRotator(Value, 0.f, 0.f).Quaternion());
}