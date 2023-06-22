#include "HeightMapGenerator_Tri.h"
#include "HeightMapSettings_Tri.h"

UHeightMap_Tri* HeightMapGenerator_Tri::GenerateHeightMap(int width, int height, UHeightMapSettings_Tri* settings, FVector2D sampleCenter, float meshScale)
{
	TArray<TArray<float>> values = NoiseMap::GenerateNoiseMap(width, height, settings->NoiseSettings, sampleCenter, false);
	
	float maxValue = TNumericLimits<float>::Min();
	float minValue = TNumericLimits<float>::Max();

	for (int y = 0; y < width; y++) {
		for (int x = 0; x < height;x++) {
			values[x][y] *= settings->HeightCurve->GetFloatValue(values[x][y]) * settings->HeightMultiplier * meshScale;
		
			if (values[x][y] > maxValue) {
				maxValue = values[x][y];
			}
			if (values[x][y] < minValue) {
				minValue = values[x][y];
			}
		}
	}
	UHeightMap_Tri* heightMap = NewObject<UHeightMap_Tri>();
	heightMap->Initialize(values, minValue, maxValue);

	return heightMap;
}
