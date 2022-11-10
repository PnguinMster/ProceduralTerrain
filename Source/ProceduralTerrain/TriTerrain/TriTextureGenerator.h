#pragma once

#include "CoreMinimal.h"

class PROCEDURALTERRAIN_API TriTextureGenerator
{
public:
	static UTexture2D* TextureFromColorMap(TArray<uint8> colorMap, int width, int height);
	static UTexture2D* TextureFromHeightMap(TArray<TArray<float>> heightMap);
};
