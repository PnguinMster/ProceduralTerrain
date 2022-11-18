#pragma once

#include "CoreMinimal.h"
#include "TriHeightMapSettings.h"
#include "TriHeightMapSettings.h"

class PROCEDURALTERRAIN_API HeightMapGenerator_Tri
{
public:
	static UTri_HeightMap* GenerateHeightMap(int width, int height, UTriHeightMapSettings* heightSettings, FVector2D sampleCenter);
};
