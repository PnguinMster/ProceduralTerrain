#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "ProceduralMeshComponent.h"
#include "HeightMapSettings_Block.h"
#include "MeshSettings_Block.h"

class PROCEDURALTERRAIN_API TextureGenerator_Block
{
public:
	static UMaterialInstanceDynamic* CreateMaterialInstance(UHeightMap_Block* heightMap, UMeshSettings_Block* meshSettings, UObject* inOuter);
	static UTexture2D* TextureFromColorMap(TArray<uint8> colorMap, int width, int height);
	static UTexture2D* TextureFromHeightMap(TArray<TArray<float>> heightMap, float heightMultiplier);
	static TArray<uint8> GenerateColorMap(TArray<TArray<float>> values, TArray<FTerrainType_Block>* regions, float chunkSize);
};
