#pragma once
#include "CoreMinimal.h"
#include "Math/Color.h"

class FColorPicker {
public:
	FColor FindNextColor();

private:
	unsigned int CurrentIndex = 0;

	const FColor ColourList[9] = {
		FColor(204, 255, 51),
		FColor(204, 153, 255),
		FColor(204, 0, 102),
		FColor(102, 51, 204),
		FColor(0, 255, 204),
		FColor(153, 0, 204),
		FColor(0, 204, 0),
		FColor(153, 255, 204)
	};
};