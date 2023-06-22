#include "HeightMapGenerator_Block.h"
#include "TextureGenerator_Block.h"

UHeightMap_Block* HeightMapGenerator_Block::GenerateHeightMap(int size, UHeightMapSettings_Block* settings, FVector2D sampleCenter)
{
	TArray<TArray<float>> values = NoiseMap::GenerateNoiseMap(size, size, settings->NoiseSettings, sampleCenter, true);

	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			values[x][y] *= settings->HeightCurve->GetFloatValue(values[x][y]) * settings->HeightMultiplier;
		}
	}

	TArray<uint8> colors = TextureGenerator_Block::GenerateColorMap(values, &settings->Regions, settings->HeightMultiplier);
	UHeightMap_Block* heightMap = NewObject<UHeightMap_Block>();
	heightMap->Initialize(values, colors);
	return heightMap;
}