#include "HeightMapGenerator_Block.h"
#include "TextureGenerator_Block.h"

UHeightMap_Block* HeightMapGenerator_Block::GenerateHeightMap(int size, UHeightMapSettings_Block* settings, FVector2D sampleCenter)
{
	TArray<TArray<float>> values = NoiseMap_Block::GenerateNoiseMap(size, size, settings->NoiseSettings, sampleCenter);

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			values[i][j] *= settings->HeightCurve->GetFloatValue(values[i][j]) * settings->HeightMultiplier;
		}
	}

	TArray<uint8> colors = TextureGenerator_Block::GenerateColorMap(values, &settings->Regions, size-2);
	UHeightMap_Block* heightMap = NewObject<UHeightMap_Block>();
	heightMap->Initialize(values, colors);
	return heightMap;
}