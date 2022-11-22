#include "HeightMapSettings_Tri.h"

float UHeightMapSettings_Tri::GetMinHeight()
{
	return heightMultiplier * heightCurve->GetFloatValue(0);
}

float UHeightMapSettings_Tri::GetMaxHeight()
{
	return heightMultiplier * heightCurve->GetFloatValue(1);
}

void UHeightMap_Tri::Initialize(TArray<TArray<float>> Values, float MinValue, float MaxValue)
{
	values = Values;
	minValue = MinValue;
	maxValue = MaxValue;
}
