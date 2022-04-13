#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Utils/ColorPicker.h"
#include "MatchGameMode.generated.h"

class ASearchPlayerStart;

UCLASS()
class FEARTHESNOWBALL_API AMatchGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMatchGameMode();

	void BeginPlay() override;

	void PostLogin(APlayerController* NewPlayer) override;
	void RestartPlayer(AController* NewPlayer) override;

	void WinGame(APlayerController* winner);

private:
	void FindSearchPlayer();
	void TryExecuteSpawning();

private:
	UPROPERTY()
	ASearchPlayerStart* SearchPlayerStart;

	TArray<AController*> WaitToSpawn;

	FColorPicker playerColor;

	bool gameWon = false;
};
