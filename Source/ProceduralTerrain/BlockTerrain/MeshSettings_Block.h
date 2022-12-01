#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MeshSettings_Block.generated.h"

USTRUCT(BlueprintType)
struct FLODInfo_Block
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1.0", UIMin = "1.0"))
		int ChunksVisible;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "6.0", UIMin = "0.0", UIMax = "6.0"))
		int Lod;
};

UCLASS()
class PROCEDURALTERRAIN_API UMeshSettings_Block : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1.0", UIMin = "1.0"))
		float MeshScale = 10.f;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1.0", UIMin = "1.0"))
		int ChunkSize = 32;
	UPROPERTY(EditDefaultsOnly)
		TArray<FLODInfo_Block> DetailLevels;
	UPROPERTY(EditDefaultsOnly)
		UMaterialInterface* MaterialInterface;

	float GetMeshWorldScale();
	float GetMeshVertsNum();
};
