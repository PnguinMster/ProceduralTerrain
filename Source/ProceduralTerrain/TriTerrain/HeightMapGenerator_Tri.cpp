#include "HeightMapGenerator_Tri.h"
#include "HeightMapSettings_Tri.h"

UHeightMap_Tri* HeightMapGenerator_Tri::GenerateHeightMap(int width, int height, UHeightMapSettings_Tri* settings, FVector2D sampleCenter)
{
	TArray<TArray<float>> values = NoiseMap_Tri::GenerateNoiseMap(width, height, settings->noiseSettings, sampleCenter);
	
	float maxValue = TNumericLimits<float>::Min();
	float minValue = TNumericLimits<float>::Max();

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height;j++) {
			values[i][j] *= settings->heightCurve->GetFloatValue(values[i][j]) * settings->heightMultiplier;
		
			if (values[i][j] > maxValue) {
				maxValue = values[i][j];
			}
			if (values[i][j] < minValue) {
				minValue = values[i][j];
			}
		}
	}
	UHeightMap_Tri* heightMap = NewObject<UHeightMap_Tri>();
	heightMap->Initialize(values, minValue, maxValue);

	return heightMap;
}
