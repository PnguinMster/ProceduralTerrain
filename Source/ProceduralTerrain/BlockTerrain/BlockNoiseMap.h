#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "BlockHeightMapSettings.h"
#include "CoreMinimal.h"

class PROCEDURALTERRAIN_API BlockNoiseMap
{
public:
	static TArray<TArray<float>>GenerateNoiseMap(int mapWidth, int mapHeight, FNoiseSettings settings, FVector2D sampleCenter);

};
