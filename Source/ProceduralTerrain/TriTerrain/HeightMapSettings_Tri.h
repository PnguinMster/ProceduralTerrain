#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NoiseMap_Tri.h"
#include "HeightMapSettings_Tri.generated.h"

USTRUCT()
struct FTerrainType_Tri {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float Height;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float Blend;
	UPROPERTY(EditAnywhere)
		FColor Color;
};

UCLASS()
class PROCEDURALTERRAIN_API UHeightMapSettings_Tri : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	FNoiseSettings_Tri NoiseSettings;

	UPROPERTY(EditDefaultsOnly)
		float HeightMultiplier = 300.f;
	UPROPERTY(EditDefaultsOnly)
		UCurveFloat* HeightCurve;
	UPROPERTY(EditDefaultsOnly)
		TArray<FTerrainType_Tri> Regions;

	float GetMinHeight();
	float GetMaxHeight();
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