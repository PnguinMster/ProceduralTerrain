#include "HeightMapSettings_Tri.h"

float UHeightMapSettings_Tri::GetMinHeight()
{
	return HeightMultiplier * HeightCurve->GetFloatValue(0);
}

float UHeightMapSettings_Tri::GetMaxHeight()
{
	return HeightMultiplier * HeightCurve->GetFloatValue(1);
}

void UHeightMap_Tri::Initialize(TArray<TArray<float>> values, float minValue, float maxValue)
{
	Values = values;
	MinValue = minValue;
	MaxValue = maxValue;
}
