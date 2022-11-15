#pragma once

#include "CoreMinimal.h"
#include "TriMapThreading.h"
#include "TriHeightMapSettings.h"

class PROCEDURALTERRAIN_API HeightMapGenerator_Tri
{
public:
	static FTri_HeightMap GenerateHeightMap(int width, int height, UTriHeightMapSettings* settings, FVector2D sampleCenter);
};
