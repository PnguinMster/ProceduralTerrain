#pragma once

#include "CoreMinimal.h"
#include "NoiseMap.generated.h"

USTRUCT(BlueprintType)
struct FNoiseSettings
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.01", UIMin = "0.01"))
		float Scale = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "0.0", UIMin = "0.0"))
		int Octaves = 6;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float Persistance = 0.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "1.0", UIMin = "1.0"))
		float Lacunarity = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int Seed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FVector2D Offset;
};

class PROCEDURALTERRAIN_API NoiseMap
{
public:
	static TArray<TArray<float>> GenerateNoiseMap(int mapWidth, int mapHeight, FNoiseSettings settings, FVector2D sampleCenter, bool isBlock);
	static void Set2DArrayNum(TArray<TArray<float>>& array, int width, int height);
	static void Set2DArrayNum(TArray<TArray<int>>& array, int width, int height);
};
