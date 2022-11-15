#include "TriNoiseMap.h"
#include "Kismet/KismetMathLibrary.h"

void TriNoiseMap::Set2DArrayNum(TArray<TArray<float>>& array, int width, int height)
{
	array.SetNum(width);

	for (int i = 0; i < width; i++)
		array[i].SetNum(height);
}

TArray<TArray<float>> TriNoiseMap::GenerateNoiseMap(int mapWidth, int mapHeight, FTri_NoiseSettings settings, FVector2D sampleCenter)
{
	TArray<TArray<float>> noiseMap;
	Set2DArrayNum(noiseMap, mapWidth, mapHeight);

	FRandomStream* prng = new FRandomStream(settings.seed);
	TArray<FVector2D> ocataveOffsets;
	ocataveOffsets.SetNum(settings.octaves);
	float maxPossibleHeight = 0;
	float amplitude = 1;
	float frequency = 1;

	for (int i = 0; i < settings.octaves; i++) {
		float offsetX = prng->FRandRange(-100000, 100000) + settings.offset.X + sampleCenter.X;
		float offsetY = prng->FRandRange(-100000, 100000) - settings.offset.Y - sampleCenter.Y;
		ocataveOffsets[i].Set(offsetX, offsetY);

		maxPossibleHeight += amplitude;
		amplitude *= settings.persistance;
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

			for (int i = 0; i < settings.octaves; i++) {
				FVector2D sample = FVector2D((x - halfWidth+ ocataveOffsets[i].X) / settings.scale * frequency , (y - halfHeight + ocataveOffsets[i].Y) / settings.scale * frequency);

				//Can remove *2 and -1 for variance
				float perlinValue = FMath::PerlinNoise2D(sample) * 2.f - 1;
				noiseHeight += perlinValue * amplitude;

				amplitude *= settings.persistance;
				frequency *= settings.lacunarity;
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
