#pragma once

#include "CoreMinimal.h"
#include "NoiseMap_Tri.generated.h"


USTRUCT()
struct FNoiseSettings_Tri
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

class PROCEDURALTERRAIN_API NoiseMap_Tri
{
public:
	static void Set2DArrayNum(TArray<TArray<float>>& array, int width, int height);
	static TArray<TArray<float>> GenerateNoiseMap(int mapWidth, int mapHeight, FNoiseSettings_Tri settings, FVector2D sampleCenter);
};