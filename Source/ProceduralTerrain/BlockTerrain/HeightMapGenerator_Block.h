#pragma once

#include "CoreMinimal.h"
#include "HeightMapSettings_Block.h"

class PROCEDURALTERRAIN_API HeightMapGenerator_Block
{
public:
	static UHeightMap_Block* GenerateHeightMap(int size, UHeightMapSettings_Block* settings, FVector2D sampleCenter);
};
