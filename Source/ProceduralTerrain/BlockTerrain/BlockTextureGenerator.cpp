#include "BlockTextureGenerator.h"

UTexture2D* BlockTextureGenerator::TextureFromColorMap(TArray<uint8> colorMap, int width, int height)
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

UTexture2D* BlockTextureGenerator::TextureFromHeightMap(TArray<TArray<float>> heightMap, float heightMultiplier)
{
	int width = heightMap.Num()-2;
	int height = heightMap[0].Num()-2;

	TArray<uint8> colorMap;
	colorMap.SetNum(width * height * 4);

	//Add Perlin Noise Pixel to Texture
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			FColor color = UKismetMathLibrary::LinearColorLerp(FLinearColor::Black, FLinearColor::White, heightMap[x+1][y+1]/ heightMultiplier).ToFColor(true);
			int currentPixelIndex = y * width + x;
			colorMap[4 * currentPixelIndex] = color.B;
			colorMap[4 * currentPixelIndex + 1] = color.G;
			colorMap[4 * currentPixelIndex + 2] = color.R;
			colorMap[4 * currentPixelIndex + 3] = color.A;
		}
	}
	return TextureFromColorMap(colorMap, width, height);
}

void BlockTextureGenerator::DrawTexture(UTexture2D* texture, UMaterialInterface* matInterface, UProceduralMeshComponent* meshComponent)
{
	//Add Texture to Material
	UMaterialInstanceDynamic* dynamicMaterialInstance = UMaterialInstanceDynamic::Create(matInterface, meshComponent->GetOwner());
	dynamicMaterialInstance->SetTextureParameterValue("ChunkTexture", texture);
	meshComponent->SetMaterial(0, dynamicMaterialInstance);
}

TArray<uint8> BlockTextureGenerator::GenerateColorMap(TArray<TArray<float>> values, TArray<FTerrainType>* regions, float heightMultiplier)
{
	int width = values.Num() - 2;
	int height = values[0].Num() - 2;
	//Create Color Map
	TArray<uint8> colorMap;
	colorMap.SetNum(width * height * 4);

	//Add Appropriate Color To Each Pixel Array
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float currentHeight = values[x+1][y+1];

			for (int i = 0; i < regions->Num(); i++) {
				if (currentHeight >= (*regions)[i].height * heightMultiplier) {
					int currentPixelIndex = y * height + x;
					colorMap[4 * currentPixelIndex] = (*regions)[i].color.B;
					colorMap[4 * currentPixelIndex + 1] = (*regions)[i].color.G;
					colorMap[4 * currentPixelIndex + 2] = (*regions)[i].color.R;
					colorMap[4 * currentPixelIndex + 3] = (*regions)[i].color.A;
				}
				else
					break;
			}
		}
	}
	return colorMap;
}
