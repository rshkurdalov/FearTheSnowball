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

public:

	UPROPERTY(VisibleAnywhere)
	USphereComponent* CaptureCollider;

	UPROPERTY(EditDefaultsOnly)
	UDecalComponent* Decal;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0"))
	float CaptureAreaRadius;

	UPROPERTY(EditAnywhere)
	FColor NeutralColor = { 61, 61, 61 };

	UFUNCTION(BlueprintImplementableEvent)
	void OnPointStateChange(float ChangingSpeed, FColor PointColor);

protected:
	void UpdateAreaRadius();
};