#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MatchPlayerState.generated.h"

UCLASS()
class FEARTHESNOWBALL_API AMatchPlayerState : public APlayerState
{
	GENERATED_BODY()
		
public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	FColor Color = FColor::Red;

protected:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};