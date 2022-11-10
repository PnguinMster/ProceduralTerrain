#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BlockMeshSettings.generated.h"

UENUM(BlueprintType)
enum class EDrawMode : uint8 {
	NoiseMap UMETA(DisplayName = "Noise Map"),
	ColorMap  UMETA(DisplayName = "Color Map"),
};

USTRUCT(BlueprintType)
struct FTerrainType
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float height;
	UPROPERTY(EditAnywhere)
		FColor color;
};

UCLASS()
class PROCEDURALTERRAIN_API UBlockMeshSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0", UIMin = "1.0"))
		int BlockSize = 32;

	UPROPERTY(EditAnywhere)
		EDrawMode drawMode = EDrawMode::ColorMap;
	UPROPERTY(EditAnywhere)
		TArray<FTerrainType> regions;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0", UIMin = "1.0"))
		int chunkSize = 32;
	
};
