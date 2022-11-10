#pragma once

#include "CoreMinimal.h"

class PROCEDURALTERRAIN_API TriNoiseMap
{
public:
	static void Set2DArrayNum(TArray<TArray<float>>& array, int width, int height);
	static TArray<TArray<float>> GenerateNoiseMap(int mapWidth, int mapHeight, int seed, float scale, int ocataves, float persistance, float lacunarity, FVector2D offset);
};
