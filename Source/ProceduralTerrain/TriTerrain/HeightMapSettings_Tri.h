#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProceduralTerrain/Terrain/NoiseMap.h"
#include "HeightMapSettings_Tri.generated.h"

USTRUCT(BlueprintType)
struct FTerrainType_Tri 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float Height;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float Blend;
	UPROPERTY(EditDefaultsOnly)
		FColor Color;
};

UCLASS(BlueprintType)
class PROCEDURALTERRAIN_API UHeightMapSettings_Tri : public UPrimaryDataAsset
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
		TArray<FTerrainType_Tri> Regions;
};

UCLASS()
class PROCEDURALTERRAIN_API UHeightMap_Tri : public UObject
{
	GENERATED_BODY()

public:
	TArray<TArray<float>> Values;
	float MinValue;
	float MaxValue;

	UHeightMap_Tri() {};
	void Initialize(TArray<TArray<float>> values, float minValue, float maxValue);
};