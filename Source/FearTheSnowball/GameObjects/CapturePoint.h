#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CapturePoint.generated.h"

class USphereComponent;
class UDecalComponent;

UCLASS()
class FEARTHESNOWBALL_API ACapturePoint : public AActor
{
	GENERATED_BODY()

public:
	ACapturePoint();

	void PostEditChangeProperty(FPropertyChangedEvent& e) override;

	void BeginPlay() override;

	void Tick(float DeltaSeconds) override;

public:

	UPROPERTY(VisibleAnywhere)
	USphereComponent* CaptureCollider;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UDecalComponent* Decal;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0"))
	float CaptureAreaRadius = 200.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0"))
	float CapturingPoints = 1000.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0"))
	float CapturingSpeed = 20.f;

	UPROPERTY(EditAnywhere)
	FColor NeutralColor = { 61, 61, 61 };

	UFUNCTION(BlueprintImplementableEvent)
	void OnPointRadiusChange(float ChangeDirection, float NewFillRadius, FColor PointColor);

protected:

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void UpdateAreaRadius();

	APlayerController* GetPlayerController(AActor* OtherActor);
	FColor GetPlayerColor(APlayerController* controller);

	void TickServer(float DeltaSeconds);
	
	void TickClient(float DeltaSeconds);

	void RecalculateCapturingState(bool forceUpdate = false);

	UFUNCTION(NetMulticast, Reliable)
	void UpdateClientCapturingState(int32 growingState, float progress, FColor color);

	void UpdateProgress(float DeltaTime);
	
	void HandleProgress();

private:
	// <player id, FColor>
	TMap<APlayerController*, FColor> InPointPlayers;
	int32 growingDirection = 0;

	struct {
		APlayerController* player;
		FColor color = FColor(16.f, 16.f, 16.f);
		float progress = 0.f;
	} CurrentCapturing;
};