#pragma once

#include "CoreMinimal.h"
#include "BlockNoiseMap.h"
#include "BlockHeightMapSettings.h"

class PROCEDURALTERRAIN_API BlockHeightMapGenerator
{
public:
	static FHeightMap GenerateHeightMap(int width, int height, UBlockHeightMapSettings* settings, FVector2D sampleCenter);
};
