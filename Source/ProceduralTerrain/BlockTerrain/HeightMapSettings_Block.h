#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProceduralTerrain/Terrain/NoiseMap.h"
#include "HeightMapSettings_Block.generated.h"

USTRUCT(BlueprintType)
struct FTerrainType_Block
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float Height;
	UPROPERTY(EditDefaultsOnly)
		FColor Color;
};

UCLASS(BlueprintType)
class PROCEDURALTERRAIN_API UHeightMapSettings_Block : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FNoiseSettings NoiseSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "1.0", UIMin = "1.0"))
		float HeightMultiplier = 300.f;
	UPROPERTY(EditDefaultsOnly)
		UCurveFloat* HeightCurve;
	UPROPERTY(EditDefaultsOnly)
		TArray<FTerrainType_Block> Regions;
};

UCLASS()
class UHeightMap_Block : public UObject
{
	GENERATED_BODY()

public:
	TArray<TArray<float>> Values;
	TArray<uint8> Color;

	UHeightMap_Block() {};
	void Initialize(TArray<TArray<float>> values, TArray<uint8> color);
};