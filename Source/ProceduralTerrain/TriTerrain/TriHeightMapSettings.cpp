#include "TriHeightMapSettings.h"

float UTriHeightMapSettings::GetMinHeight()
{
	return heightMultiplier * heightCurve->GetFloatValue(0);
}

float UTriHeightMapSettings::GetMaxHeight()
{
	return heightMultiplier * heightCurve->GetFloatValue(1);
}
