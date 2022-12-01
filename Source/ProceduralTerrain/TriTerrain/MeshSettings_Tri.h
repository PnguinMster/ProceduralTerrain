#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MeshSettings_Tri.generated.h"

USTRUCT()
struct FLODInfo_Tri
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1.0", UIMin = "1.0"))
		int VisibleChunks;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "6.0", UIMin = "0.0", UIMax = "6.0"))
		int Lod;
};

UCLASS()
class PROCEDURALTERRAIN_API UMeshSettings_Tri : public UDataAsset
{
	GENERATED_BODY()

	UMeshSettings_Tri();
	
	static TArray<int> SupportedChunkSizes;

public:
	UPROPERTY(EditDefaultsOnly)
		float MeshScale = 10.f;
	/**
	0 = 48
	1 = 72
	2 = 96
	3 = 120
	4 = 144
	5 = 168
	6 = 192
	7 = 216
	8 = 240 
	*/
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "8.0", UIMin = "0.0", UIMax = "8.0"))
		int ChunkSizeIndex = 0;
	UPROPERTY(EditDefaultsOnly)
		TArray<FLODInfo_Tri> DetailLevels;
	UPROPERTY(EditDefaultsOnly)
		UMaterialInterface* MaterialInterface;

	int GetNumberVerticesPerLine();
	float GetMeshWorldSize();
};
