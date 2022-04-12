
#include "MatchGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/MatchPlayerState.h"
#include "GameObjects/SearchPlayerStart.h"

AMatchGameMode::AMatchGameMode() 
{
	PlayerStateClass = AMatchPlayerState::StaticClass();
}

void AMatchGameMode::BeginPlay()
{
	Super::BeginPlay();
	FindSearchPlayer();
}

void AMatchGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (auto playerState = NewPlayer->GetPlayerState<AMatchPlayerState>())
		playerState->Color = playerColor.FindNextColor();
}

void AMatchGameMode::FindSearchPlayer() 
{
	TArray<AActor*> SearchPlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASearchPlayerStart::StaticClass(), SearchPlayerStarts);

	if (SearchPlayerStarts.Num() == 0) 
	{
		UE_LOG(LogGameMode, Error, TEXT("FindSearchPlayer: Required ASearchPlayerStart on level."));
		return;
	}
	else if (SearchPlayerStarts.Num() > 1) 
	{
		UE_LOG(LogGameMode, Warning, TEXT("FindSearchPlayer: ASearchPlayerStart have to be only one."));
	}

	SearchPlayerStart = Cast<ASearchPlayerStart>(SearchPlayerStarts[0]);

	if (SearchPlayerStart->HasActorBegunPlay())
		TryExecuteSpawning();
	else
		SearchPlayerStart->OnBeginPlay.AddUObject(this, &AMatchGameMode::TryExecuteSpawning);
}

void AMatchGameMode::TryExecuteSpawning()
{
	if (!IsValid(SearchPlayerStart) || !SearchPlayerStart->HasActorBegunPlay())
		return;

	for (auto controller : WaitToSpawn) {
		FTransform spawnTransform = SearchPlayerStart->SearchNewLocation();

		// Restart has strange behavior when pawn exist.
		if (auto pawn = controller->GetPawn()) {
			pawn->Destroy();
			controller->SetPawn(nullptr);
		}

		Super::RestartPlayerAtTransform(controller, spawnTransform);
	}

	WaitToSpawn.Empty();
}

void AMatchGameMode::WinGame(APlayerController* winner)
{
	if (!winner)
		return;

	UE_LOG(LogGameMode, Warning, TEXT("Player win: %s"), *GetDebugName(winner));
}

void AMatchGameMode::RestartPlayer(AController* NewPlayer)
{
	WaitToSpawn.Add(NewPlayer);
	TryExecuteSpawning();
}