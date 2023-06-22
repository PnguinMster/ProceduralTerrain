#include "HeightMapSettings_Tri.h"

void UHeightMap_Tri::Initialize(TArray<TArray<float>> values, float minValue, float maxValue)
{
	Values = values;
	MinValue = minValue;
	MaxValue = maxValue;
}
