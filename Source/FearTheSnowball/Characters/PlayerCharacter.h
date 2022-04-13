#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class UEndGameWidget;

UCLASS()
class FEARTHESNOWBALL_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USceneComponent* BaseCameraRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UEndGameWidget> EndGameWidgetClass;

	UFUNCTION(Client, Reliable)
	void EndGame(bool isWinner);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void MoveForward(float Value);

	UFUNCTION()
	void MoveBackward(float Value);

	UFUNCTION()
	void MoveLeft(float Value);

	UFUNCTION()
	void MoveRight(float Value);

	UFUNCTION()
	void LookVertical(float Value);

	UPROPERTY()
	UEndGameWidget* EndGameWidget;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
