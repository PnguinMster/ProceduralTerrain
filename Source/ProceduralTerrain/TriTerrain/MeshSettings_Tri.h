#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MeshSettings_Tri.generated.h"

USTRUCT()
struct FLODInfo_Tri
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0", UIMin = "1.0"))
		int visibleChunks;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "6.0", UIMin = "0.0", UIMax = "6.0"))
		int lod;
};

UCLASS()
class PROCEDURALTERRAIN_API UMeshSettings_Tri : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
		float meshScale = 10.f;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1.0", UIMin = "1.0"))
		int numberOfVerticesPerLine = 77;
	UPROPERTY(EditDefaultsOnly)
		TArray<FLODInfo_Tri> detailLevels;
	UPROPERTY(EditDefaultsOnly)
		UMaterialInterface* materialInterface;

	float GetMeshWorldSize();
};
