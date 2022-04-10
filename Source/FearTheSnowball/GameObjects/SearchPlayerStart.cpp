
#include "SearchPlayerStart.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Characters/PlayerCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/GeometryUtils.h"

ASearchPlayerStart::ASearchPlayerStart()
{
	GenerateEditorVisualizers_Constructor();
}

void ASearchPlayerStart::BeginPlay() 
{
	Super::BeginPlay();
	OnBeginPlay.Broadcast();
}

#if WITH_EDITOR

void ASearchPlayerStart::PostEditChangeProperty(FPropertyChangedEvent& e) 
{
	UpdateEditorVisualizers();
	Super::PostEditChangeProperty(e);
}

void ASearchPlayerStart::GenerateEditorVisualizers_Constructor()
{
	InnerRadiusVisualizer = CreateDefaultSubobject<USphereComponent>("InnerRadiusVisualizer");
	InnerRadiusVisualizer->SetupAttachment(RootComponent);
	InnerRadiusVisualizer->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InnerRadiusVisualizer->bDrawOnlyIfSelected = true;

	OuterRadiusVisualizer = CreateDefaultSubobject<USphereComponent>("OuterRadiusVisualizer");
	OuterRadiusVisualizer->SetupAttachment(RootComponent);
	OuterRadiusVisualizer->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OuterRadiusVisualizer->bDrawOnlyIfSelected = true;

	UpdateEditorVisualizers();
}

void ASearchPlayerStart::UpdateEditorVisualizers() 
{
	if (!InnerRadiusVisualizer || !OuterRadiusVisualizer) 
	{
		UE_LOG(LogExec, Warning, TEXT("UpdateEditorVisualizers: Visualizer components are invalid."));
		return;
	}

	InnerRadiusVisualizer->SetSphereRadius(InnerRadius, false);
	OuterRadiusVisualizer->SetSphereRadius(OuterRadius, false);
}
#endif

FTransform ASearchPlayerStart::SearchNewLocation() const
{
	//auto playerClass = GetWorld()->GetAuthGameMode()->DefaultPawnClass;
	TArray<AActor*> players;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), players);

	// Arc math is counter-clockwise in radians [0 ; 2 * PI]
	// {1, 0} == 0 rad
	const float PI_2 = (2.0 * PI);

	TArray<FFloatInterval> restrictedArcs;
	{
		const FVector2D basePoint = { GetActorLocation().X, GetActorLocation().Y};

		for (const auto player : players) 
		{
			const FVector2D playerPoint = { player->GetActorLocation().X, player->GetActorLocation().Y };
			const FVector2D localPlayerPoint = playerPoint - basePoint;
			const float distanceToPlayer = localPlayerPoint.Size();

			if (!FMath::IsWithin(distanceToPlayer, InnerRadius, OuterRadius))
				continue;

			const float sinOfHalfArcLength = RestrictedPlayerRadius / localPlayerPoint.Size();
			const float halfArcLength = FMath::Asin(sinOfHalfArcLength);

			const float middleOfArc = UGeometryUtilsStatics::GetAngleCounterClockwise(localPlayerPoint);

			{
				float bottomInterval = middleOfArc - halfArcLength;
				float upperInterval = middleOfArc + halfArcLength;

				const bool requiredSplitByBottom = bottomInterval < 0.f;
				const bool requiredSplitByUpper = upperInterval > PI_2;

				if (requiredSplitByBottom) // Required split by bottom.
				{
					const float convertedBottom = PI_2 - FMath::Abs(bottomInterval);
					restrictedArcs.Add({ convertedBottom, PI_2 });
					bottomInterval = 0.f;
				} 
				
				if (requiredSplitByUpper) // Required split by upper.
				{
					const float convertedUpper = FMath::Abs(upperInterval) - PI_2;
					restrictedArcs.Add({ 0.f, convertedUpper });
					upperInterval = PI_2;
				} 
				
				const bool isCoverHoleCircle = !(requiredSplitByBottom && requiredSplitByUpper);
				checkf(isCoverHoleCircle, TEXT("Interval can`t cover whole circle."));

				restrictedArcs.Add({ bottomInterval, upperInterval });
			}
		}
	}

	restrictedArcs.Sort([](const FFloatInterval& A, const FFloatInterval& B) -> bool {
		return A.Min < B.Min;
	});

	// Merge restricted.
	TArray<FFloatInterval> restrictedMergedArcs;
	{
		bool setNextCalcInterval = true;
		FFloatInterval calculatingInterval;
		
		for (int i = 0; i < restrictedArcs.Num(); ++i) 
		{
			if (setNextCalcInterval)
			{
				calculatingInterval = restrictedArcs[i];
				setNextCalcInterval = false;
				continue;
			}

			const FFloatInterval tryEndInterval = restrictedArcs[i];
			const bool isIntersecting = 
				calculatingInterval.Contains(tryEndInterval.Min) 
				|| calculatingInterval.Contains(tryEndInterval.Max);

			if (isIntersecting) 
			{
				calculatingInterval.Max = tryEndInterval.Max;
			}
			else 			
			{
				restrictedMergedArcs.Add(calculatingInterval);
				setNextCalcInterval = true;
				i -= 1;
			}
		}

		if (restrictedArcs.Num())
			restrictedMergedArcs.Add(calculatingInterval);
	}

	float spawnAngle = PI;
	if (restrictedMergedArcs.Num())
	{
		// First candidate intersect 0 if possible; 0 length in case of restricted arc intersected by 0 due to splitting.
		// Range become [-2 * PI ; 0 ; 2 * PI]
		FFloatInterval longestAcceptableArc = { restrictedMergedArcs.Last().Max - PI_2, restrictedMergedArcs[0].Min };

		for (int i = 0; i < restrictedMergedArcs.Num() - 1; ++i) {
			const FFloatInterval currentAcceptable = { restrictedMergedArcs[i].Max, restrictedMergedArcs[i + 1].Min };

			if (currentAcceptable.Size() > longestAcceptableArc.Size()) 				
				longestAcceptableArc = currentAcceptable;
		}

		const float middleSpawnPoint = (longestAcceptableArc.Min + longestAcceptableArc.Max) / 2;
		
		// Convert to [0 ; 2 * PI]
		spawnAngle = middleSpawnPoint > 0.f ? middleSpawnPoint : middleSpawnPoint + PI_2;
	}

	// Generate transform.
	FTransform resultTransform;
	{
		const FRotator spawnWay = FRotator(0.f, FMath::RadiansToDegrees(spawnAngle), 0.f);

		UE_LOG(LogTemp, Warning, TEXT("Debug: %f"), spawnWay.Yaw);

		const FVector spawnDirection = spawnWay.RotateVector({ 1.f, 0.f, 0.f });
		const float spawnDistance = FMath::RandRange(InnerRadius, OuterRadius);
		const FVector localSpawnPoint = spawnDistance * spawnDirection;

		const FVector spawnPoint = GetActorLocation() + localSpawnPoint;
		const FRotator spawnFaceRotation = spawnWay + FRotator(0.f, 180.f, 0.f);
		
		resultTransform.SetLocation(spawnPoint);
		resultTransform.SetRotation(spawnFaceRotation.Quaternion());
	}

	return resultTransform;
}