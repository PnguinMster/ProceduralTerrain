#pragma once

#include "CoreMinimal.h"
#include "HeightMapSettings_Tri.h"
#include "MeshSettings_Tri.h"

class PROCEDURALTERRAIN_API TextureGenerator_Tri
{
public:
	static UMaterialInstanceDynamic* CreateMaterialInstance(UMeshSettings_Tri* meshSettings, UHeightMapSettings_Tri* heightMapSettings, UObject* inOuter);
	static UTexture2D* ColorArrayToTexture(TArray<FColor> colors);
	static UTexture2D* FloatArrayToTexture(TArray<float> convertArray);
	static UTexture2D* TextureFromColorMap(TArray<uint8> colorMap, int width, int height);
	static UTexture2D* TextureFromHeightMap(TArray<TArray<float>> heightMap);
};
