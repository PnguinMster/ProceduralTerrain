#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NoiseMap_Tri.h"
#include "HeightMapSettings_Tri.generated.h"

USTRUCT()
struct FTerrainType_Tri {
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float height;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float blend;
	UPROPERTY(EditAnywhere)
		FColor color;
};

UCLASS()
class PROCEDURALTERRAIN_API UHeightMapSettings_Tri : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	FNoiseSettings_Tri noiseSettings;

	UPROPERTY(EditDefaultsOnly)
		float heightMultiplier = 300.f;
	UPROPERTY(EditDefaultsOnly)
		UCurveFloat* heightCurve;
	UPROPERTY(EditDefaultsOnly)
		TArray<FTerrainType_Tri> regions;

	float GetMinHeight();
	float GetMaxHeight();
};

UCLASS()
class PROCEDURALTERRAIN_API UHeightMap_Tri : public UObject
{
	GENERATED_BODY()

public:
	TArray<TArray<float>> values;
	float minValue;
	float maxValue;

	UHeightMap_Tri() {};
	void Initialize(TArray<TArray<float>> Values, float MinValue, float MaxValue);
};