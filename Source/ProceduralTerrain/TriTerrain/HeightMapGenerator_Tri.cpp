#include "HeightMapGenerator_Tri.h"
#include "TriNoiseMap.h"

UTri_HeightMap* HeightMapGenerator_Tri::GenerateHeightMap(int width, int height, UTriHeightMapSettings* settings, FVector2D sampleCenter)
{
	TArray<TArray<float>> values = TriNoiseMap::GenerateNoiseMap(width, height, settings->noiseSettings, sampleCenter);
	
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
	UTri_HeightMap* heightMap = NewObject<UTri_HeightMap>();
	heightMap->Initialize(values, minValue, maxValue);

	return heightMap;
}
