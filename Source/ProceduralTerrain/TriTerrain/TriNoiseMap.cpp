#include "TriNoiseMap.h"
#include "Kismet/KismetMathLibrary.h"

void TriNoiseMap::Set2DArrayNum(TArray<TArray<float>>& array, int width, int height)
{
	array.SetNum(width);

	for (int i = 0; i < width; i++)
		array[i].SetNum(height);
}

TArray<TArray<float>> TriNoiseMap::GenerateNoiseMap(int mapWidth, int mapHeight, int seed, float scale, int octaves, float persistance, float lacunarity , FVector2D offset)
{
	TArray<TArray<float>> noiseMap;
	Set2DArrayNum(noiseMap, mapWidth, mapHeight);

	FRandomStream* prng = new FRandomStream(seed);
	TArray<FVector2D> ocataveOffsets;
	ocataveOffsets.SetNum(octaves);
	float maxPossibleHeight = 0;
	float amplitude = 1;
	float frequency = 1;

	for (int i = 0; i < octaves; i++) {
		float offsetX = prng->FRandRange(-100000, 100000) + offset.X;
		float offsetY = prng->FRandRange(-100000, 100000) - offset.Y;
		ocataveOffsets[i].Set(offsetX, offsetY);

		maxPossibleHeight += amplitude;
		amplitude *= persistance;
	}

	if (scale < 0)
		scale = 0.001f;
	

	float maxLocalNoiseHeight = TNumericLimits<float>::Min();
	float minLocalNoiseHeight = TNumericLimits<float>::Max();

	float halfWidth = mapWidth / 2.f;
	float halfHeight = mapHeight / 2.f;

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			amplitude = 1;
			frequency = 1;
			float noiseHeight = 0;

			for (int i = 0; i < octaves; i++) {
				FVector2D sample = FVector2D((x - halfWidth+ ocataveOffsets[i].X) / scale * frequency , (y - halfHeight + ocataveOffsets[i].Y) / scale * frequency);

				//Can remove *2 and -1 for variance
				float perlinValue = FMath::PerlinNoise2D(sample) * 2.f - 1;
				noiseHeight += perlinValue * amplitude;

				amplitude *= persistance;
				frequency *= lacunarity;
			}

			if (noiseHeight > maxLocalNoiseHeight)
				maxLocalNoiseHeight = noiseHeight;
			else if (noiseHeight < minLocalNoiseHeight)
				minLocalNoiseHeight = noiseHeight;

			noiseMap[x][y] = noiseHeight;
		}
	}

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			//noiseMap[x][y] = UKismetMathLibrary::NormalizeToRange(noiseMap[x][y], minNoiseHeight, maxNoiseHeight);
			float normilizedHeight = (noiseMap[x][y] + 2) / (maxPossibleHeight / 0.9f);
			noiseMap[x][y] = FMath::Clamp(normilizedHeight, 0, TNumericLimits<float>::Max());
		}
	}
	return noiseMap;
}
