#include "BlockNoiseMap.h"

TArray<TArray<float>> BlockNoiseMap::GenerateNoiseMap(int mapWidth, int mapHeight, FNoiseSettings settings, FVector2D sampleCenter)
{
	//Variables
	TArray<TArray<float>> noiseMap;
	FRandomStream* prng = new FRandomStream(settings.seed);
	TArray<FVector2D> ocataveOffsets;

	float maxPossibleHeight = 0;
	float amplitude = 1.f;
	float frequency = 1.f;

	ocataveOffsets.SetNum(settings.octaves);
	//Offset Octave
	for (int i = 0; i < settings.octaves; i++) {
		float offsetX = prng->FRandRange(-100000, 100000) + settings.offset.X + sampleCenter.X;
		float offsetY = prng->FRandRange(-100000, 100000) + settings.offset.Y + sampleCenter.Y;
		ocataveOffsets[i].Set(offsetX, offsetY);

		maxPossibleHeight += amplitude;
		amplitude *= settings.persistance;
	}

	//Create 2D Array
	noiseMap.SetNum(mapWidth);
	for (int i = 0; i < mapWidth; i++)
		noiseMap[i].SetNum(mapHeight);

	float maxLocalNoiseHeight = TNumericLimits<float>::Min();
	float minLocalNoiseHeight = TNumericLimits<float>::Max();
	//Scale from Center
	float halfWidth = mapWidth / 2.f;
	float halfHeight = mapHeight / 2.f;

	//Create 2D Perlin Noise
	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			amplitude = 1.f;
			frequency = 1.f;
			float noiseHeight = 0.f;

			//Change Detail Octave
			for (int i = 0; i < settings.octaves; i++) {
				FVector2D sample = FVector2D(
					(x - halfWidth + ocataveOffsets[i].X) / settings.scale * frequency,
					(y - halfHeight + ocataveOffsets[i].Y) / settings.scale * frequency);

				float perlinValue = FMath::PerlinNoise2D(sample) * 2.f - 1.f;
				noiseHeight += perlinValue * amplitude;
				amplitude *= settings.persistance;
				frequency *= settings.lacunarity;
			}

			//Set Max Min Noise Height
			if (noiseHeight > maxLocalNoiseHeight)
				maxLocalNoiseHeight = noiseHeight;
			if (noiseHeight < minLocalNoiseHeight)
				minLocalNoiseHeight = noiseHeight;

			noiseMap[x][y] = noiseHeight;

			//normilize noise map
			float normilizedHeight = (noiseMap[x][y] + 1.f) / (-maxPossibleHeight / 0.9f);
			noiseMap[x][y] = FMath::Clamp(normilizedHeight, 0, TNumericLimits<float>::Max());
		}
	}
	return noiseMap;
}
