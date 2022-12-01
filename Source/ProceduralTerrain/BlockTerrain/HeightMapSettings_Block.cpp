#include "HeightMapSettings_Block.h"

void UHeightMap_Block::Initialize(TArray<TArray<float>> values, TArray<uint8> color)
{
	Values = values;
	Color = color;
}