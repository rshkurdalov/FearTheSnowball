#pragma once
#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"

class UGeometryUtilsStatics {
public:

	// in radians [0 ; 2 * pi]
	// base {1, 0}
	static float GetAngleCounterClockwise(FVector2D Input) {
		Input = Input.GetSafeNormal();
		
		const FVector2D XZero = { 1.f, 0.f };

		const float dot = FVector2D::DotProduct(Input, XZero);
		const float cross = FVector2D::CrossProduct(Input, XZero);
		const float unsignedAngle = FMath::Acos(dot);

		float result = unsignedAngle;
		
		if (cross > 0.0)
			result = (2 * PI) - result;
		
		return result;
	}
};