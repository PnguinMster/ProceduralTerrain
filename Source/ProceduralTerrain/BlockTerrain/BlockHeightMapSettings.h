#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BlockHeightMapSettings.generated.h"


USTRUCT()
struct FHeightMap
{
	GENERATED_BODY()

public:
	TArray<TArray<float>> values;
	TArray<uint8> color;

	FHeightMap() {};
	FHeightMap(TArray<TArray<float>> values) { this->values = values; }
};

USTRUCT(BlueprintType)
struct FNoiseSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.01", UIMin = "0.01"))
		float scale = 15.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0", UIMin = "1.0"))
		int octaves = 4;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float persistance = 0.5f;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0", UIMin = "1.0"))
		float lacunarity = 1.f;

	UPROPERTY(EditAnywhere)
		int seed = 7;
	UPROPERTY(EditAnywhere)
		FVector2D offset;
};

UCLASS()
class PROCEDURALTERRAIN_API UBlockHeightMapSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		FNoiseSettings noiseSettings;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0", UIMin = "1.0"))
		float heightMultiplier = 20.f;
	UPROPERTY(EditAnywhere)
		UCurveFloat* heightCurve;
	
};
