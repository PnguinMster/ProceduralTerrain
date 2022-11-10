#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "ProceduralMeshComponent.h"
#include "BlockMeshSettings.h"
#include "CoreMinimal.h"


class PROCEDURALTERRAIN_API BlockTextureGenerator
{
public:
	static UTexture2D* TextureFromColorMap(TArray<uint8> colorMap, int width, int height);
	static UTexture2D* TextureFromHeightMap(TArray<TArray<float>> heightMap, float heightMultiplier);

	static void DrawTexture(UTexture2D* texture, UMaterialInterface* matInterface, UProceduralMeshComponent* meshComponent);
	static TArray<uint8> GenerateColorMap(TArray<TArray<float>> values, TArray<FTerrainType>* regions, float heightMultiplier);
};
