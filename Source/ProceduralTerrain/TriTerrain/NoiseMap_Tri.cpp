#include "NoiseMap_Tri.h"
#include "Kismet/KismetMathLibrary.h"

TArray<TArray<float>> NoiseMap_Tri::GenerateNoiseMap(int mapWidth, int mapHeight, FNoiseSettings_Tri settings, FVector2D sampleCenter)
{
	TArray<TArray<float>> noiseMap;
	Set2DArrayNum(noiseMap, mapWidth, mapHeight);

	FRandomStream* prng = new FRandomStream(settings.Seed);
	TArray<FVector2D> ocataveOffsets;
	ocataveOffsets.SetNum(settings.Octaves);
	float maxPossibleHeight = 0;
	float amplitude = 1;
	float frequency = 1;

	for (int i = 0; i < settings.Octaves; i++) {
		float offsetX = prng->FRandRange(-100000, 100000) + settings.Offset.X + sampleCenter.X;
		float offsetY = prng->FRandRange(-100000, 100000) - settings.Offset.Y - sampleCenter.Y;
		ocataveOffsets[i].Set(offsetX, offsetY);

		maxPossibleHeight += amplitude;
		amplitude *= settings.Persistance;
	}

	float maxLocalNoiseHeight = TNumericLimits<float>::Min();
	float minLocalNoiseHeight = TNumericLimits<float>::Max();

	float halfWidth = mapWidth / 2.f;
	float halfHeight = mapHeight / 2.f;

	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			amplitude = 1;
			frequency = 1;
			float noiseHeight = 0;

			for (int i = 0; i < settings.Octaves; i++) {
				FVector2D sample = FVector2D((x - halfWidth+ ocataveOffsets[i].X) / settings.Scale * frequency , (y - halfHeight + ocataveOffsets[i].Y) / settings.Scale * frequency);

				//Can remove *2 and -1 for variance
				float perlinValue = FMath::PerlinNoise2D(sample) * 2.f - 1;
				noiseHeight += perlinValue * amplitude;

				amplitude *= settings.Persistance;
				frequency *= settings.Lacunarity;
			}

			if (noiseHeight > maxLocalNoiseHeight)
				maxLocalNoiseHeight = noiseHeight;
			if (noiseHeight < minLocalNoiseHeight)
				minLocalNoiseHeight = noiseHeight;

			noiseMap[x][y] = noiseHeight;

			float normilizedHeight = (noiseMap[x][y] + 2) / (maxPossibleHeight / 0.9f);
			noiseMap[x][y] = FMath::Clamp(normilizedHeight, 0, TNumericLimits<float>::Max());
		}
	}
	return noiseMap;
}

void NoiseMap_Tri::Set2DArrayNum(TArray<TArray<float>>& array, int width, int height)
{
	array.SetNum(width);

	for (int i = 0; i < width; i++)
		array[i].SetNum(height);
}

void NoiseMap_Tri::Set2DArrayNum(TArray<TArray<int>>& array, int width, int height)
{
	array.SetNum(width);

	for (int i = 0; i < width; i++)
		array[i].SetNum(height);
}