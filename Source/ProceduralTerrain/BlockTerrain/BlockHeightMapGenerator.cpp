#include "BlockHeightMapGenerator.h"

FHeightMap BlockHeightMapGenerator::GenerateHeightMap(int width, int height, UBlockHeightMapSettings* settings, FVector2D sampleCenter)
{
	TArray<TArray<float>> values = BlockNoiseMap::GenerateNoiseMap(width, height, settings->noiseSettings, sampleCenter);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			values[i][j] *= settings->heightCurve->GetFloatValue(values[i][j]) * settings->heightMultiplier;
		}
	}
	return FHeightMap(values);
}