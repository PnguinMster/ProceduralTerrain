#include "TextureGenerator_Block.h"

UTexture2D* TextureGenerator_Block::TextureFromColorMap(TArray<uint8> colorMap, int width, int height)
{
	// Create the texture
	UTexture2D* texture = UTexture2D::CreateTransient(height, width, PF_B8G8R8A8);
	texture->Filter = TextureFilter::TF_Nearest;
	texture->bNoTiling = true;
	FTexture2DMipMap* MipMap = &texture->GetPlatformData()->Mips[0];
	FByteBulkData* ImageData = &MipMap->BulkData;
	uint8* RawImageData = (uint8*)ImageData->Lock(LOCK_READ_WRITE);

	//Add Perlin Noise Pixel to Texture
	for (int i = 0; i < colorMap.Num(); i++)
		RawImageData[i] = colorMap[i];

	//Update Texture
	ImageData->Unlock();
	texture->UpdateResource();
	return texture;
}

UTexture2D* TextureGenerator_Block::TextureFromHeightMap(TArray<TArray<float>> heightMap, float heightMultiplier)
{
	int width = heightMap.Num() - 2;
	int height = heightMap[0].Num() - 2;

	TArray<uint8> colorMap;
	colorMap.SetNum(width * height * 4);

	//Add Perlin Noise Pixel to Texture
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			FColor color = UKismetMathLibrary::LinearColorLerp(FLinearColor::Black, FLinearColor::White, heightMap[x + 1][y + 1] / heightMultiplier).ToFColor(true);
			int currentPixelIndex = y * width + x;
			colorMap[4 * currentPixelIndex] = color.B;
			colorMap[4 * currentPixelIndex + 1] = color.G;
			colorMap[4 * currentPixelIndex + 2] = color.R;
			colorMap[4 * currentPixelIndex + 3] = color.A;
		}
	}
	return TextureFromColorMap(colorMap, width, height);
}

UMaterialInstanceDynamic* TextureGenerator_Block::CreateMaterialInstance(UHeightMap_Block* heightMap, UMeshSettings_Block* meshSettings, UObject* inOuter)
{
	UTexture2D* texture = TextureGenerator_Block::TextureFromColorMap(heightMap->Color, meshSettings->ChunkSize, meshSettings->ChunkSize);
	UMaterialInstanceDynamic* dynamicMaterialInstance = UMaterialInstanceDynamic::Create(meshSettings->MaterialInterface, inOuter);
	dynamicMaterialInstance->SetTextureParameterValue("ChunkTexture", texture);
	return dynamicMaterialInstance;
}

TArray<uint8> TextureGenerator_Block::GenerateColorMap(TArray<TArray<float>> values, TArray<FTerrainType_Block>* regions, float chunkSize)
{
	int width = values.Num() - 2;
	int height = values[0].Num() - 2;
	//Create Color Map
	TArray<uint8> colorMap;
	colorMap.SetNum(width * height * 4);

	//Add Appropriate Color To Each Pixel Array
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float currentHeight = values[x + 1][y + 1];

			for (int i = 0; i < regions->Num(); i++) {
				if (currentHeight >= (*regions)[i].Height * chunkSize) {
					int currentPixelIndex = y * height + x;
					colorMap[4 * currentPixelIndex] = (*regions)[i].Color.B;
					colorMap[4 * currentPixelIndex + 1] = (*regions)[i].Color.G;
					colorMap[4 * currentPixelIndex + 2] = (*regions)[i].Color.R;
					colorMap[4 * currentPixelIndex + 3] = (*regions)[i].Color.A;
				}
				else
					break;
			}
		}
	}
	return colorMap;
}