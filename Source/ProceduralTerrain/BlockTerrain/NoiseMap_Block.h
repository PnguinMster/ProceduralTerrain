#pragma once

#include "CoreMinimal.h"
#include "NoiseMap_Block.generated.h"

USTRUCT(BlueprintType)
struct FNoiseSettings_Block
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.01", UIMin = "0.01"))
		float Scale = 50.f;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1.0", UIMin = "1.0"))
		int Octaves = 6;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float Persistance = 0.5f;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1.0", UIMin = "1.0"))
		float Lacunarity = 2.f;

	UPROPERTY(EditDefaultsOnly)
		int Seed = 7;
	UPROPERTY(EditDefaultsOnly)
		FVector2D Offset;
};

class PROCEDURALTERRAIN_API NoiseMap_Block
{
public:
	static TArray<TArray<float>>GenerateNoiseMap(int mapWidth, int mapHeight, FNoiseSettings_Block settings, FVector2D sampleCenter);
};
