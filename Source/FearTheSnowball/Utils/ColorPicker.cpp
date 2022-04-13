#include "ColorPicker.h"

FColor FColorPicker::FindNextColor() 
{
	FColor toReturn = ColourList[CurrentIndex];

	CurrentIndex += 1;

	if (CurrentIndex >= sizeof(ColourList))
	{
		CurrentIndex = 0;
	}

	return toReturn;
}