#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "SearchPlayerStart.generated.h"

class USphereComponent;

DECLARE_MULTICAST_DELEGATE(FBeginPlay);

UCLASS()
class FEARTHESNOWBALL_API ASearchPlayerStart : public AActor 
{
	GENERATED_BODY()
		
public:
	ASearchPlayerStart();

	FTransform SearchNewLocation() const;

public:
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0"), Category = "Spawn")
	float InnerRadius = 100.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0"), Category = "Spawn")
	float OuterRadius = 1000.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0"), Category = "Spawn")
	float RestrictedPlayerRadius = 100.f;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* InnerRadiusVisualizer;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* OuterRadiusVisualizer;

	FBeginPlay OnBeginPlay;

protected:
	void BeginPlay() override;

	void PostEditChangeProperty(FPropertyChangedEvent& e) override;

	void GenerateEditorVisualizers_Constructor();
	void UpdateEditorVisualizers();
};
