#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MatchGameMode.generated.h"

class ASearchPlayerStart;

UCLASS()
class FEARTHESNOWBALL_API AMatchGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	void BeginPlay() override;

	void RestartPlayer(AController* NewPlayer) override;

private:
	void FindSearchPlayer();
	void TryExecuteSpawning();

private:
	UPROPERTY()
	ASearchPlayerStart* SearchPlayerStart;

	TArray<AController*> WaitToSpawn;
};
