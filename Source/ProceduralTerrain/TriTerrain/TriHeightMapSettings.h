#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TriHeightMapSettings.generated.h"

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

	float GetMinHeight();
	float GetMaxHeight();
};
