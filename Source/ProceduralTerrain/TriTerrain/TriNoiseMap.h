#pragma once

#include "CoreMinimal.h"
#include "TriHeightMapSettings.h"

class PROCEDURALTERRAIN_API TriNoiseMap
{
public:
	static void Set2DArrayNum(TArray<TArray<float>>& array, int width, int height);
	static TArray<TArray<float>> GenerateNoiseMap(int mapWidth, int mapHeight, FTri_NoiseSettings settings, FVector2D sampleCenter);
};