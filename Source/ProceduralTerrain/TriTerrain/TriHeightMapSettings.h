#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TriHeightMapSettings.generated.h"

UCLASS()
class PROCEDURALTERRAIN_API UTri_HeightMap : public UObject
{
	GENERATED_BODY()

public:
	TArray<TArray<float>> values;
	float minValue;
	float maxValue;

	UTri_HeightMap() {};
	void Initialize(TArray<TArray<float>> Values, float MinValue, float MaxValue) {
		values = Values;
		minValue = MinValue;
		maxValue = MaxValue;
	}
};

USTRUCT()
struct FTriTerrainType {
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float height;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float blend;
	UPROPERTY(EditAnywhere)
		FColor color;
};

USTRUCT()
struct FTri_NoiseSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.01", UIMin = "0.01"))
		float scale = 50.f;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", UIMin = "0.0"))
		int octaves = 6;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float persistance = 0.5f;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1.0", UIMin = "1.0"))
		float lacunarity = 2.f;

	UPROPERTY(EditDefaultsOnly)
		int seed;
	UPROPERTY(EditDefaultsOnly)
		FVector2D offset;
};

UCLASS()
class PROCEDURALTERRAIN_API UTriHeightMapSettings : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	FTri_NoiseSettings noiseSettings;

	UPROPERTY(EditDefaultsOnly)
		float heightMultiplier = 5.f;
	UPROPERTY(EditDefaultsOnly)
		UCurveFloat* heightCurve;
	UPROPERTY(EditDefaultsOnly)
		TArray<FTriTerrainType> regions;

	float GetMinHeight();
	float GetMaxHeight();
};
