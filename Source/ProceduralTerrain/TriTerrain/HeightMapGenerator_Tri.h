#pragma once

#include "CoreMinimal.h"
#include "HeightMapSettings_Tri.h"

class PROCEDURALTERRAIN_API HeightMapGenerator_Tri
{
public:
	static UHeightMap_Tri* GenerateHeightMap(int width, int height, UHeightMapSettings_Tri* heightSettings, FVector2D sampleCenter, float meshScale);
};
