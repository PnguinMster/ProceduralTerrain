#include "TriTextureGenerator.h"
#include "Kismet/KismetMathLibrary.h"

UTexture2D* TriTextureGenerator::TextureFromColorMap(TArray<uint8> colorMap, int width, int height)
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

UTexture2D* TriTextureGenerator::TextureFromHeightMap(TArray<TArray<float>> heightMap)
{
	int width = heightMap.Num();
	int height = heightMap[0].Num();

	TArray<uint8> colorMap;
	colorMap.SetNum(width * height * 4);

	//Add Perlin Noise Pixel to Texture
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			FColor color = UKismetMathLibrary::LinearColorLerp(FLinearColor::Black, FLinearColor::White, heightMap[x][y]).ToFColor(true);
			int currentPixelIndex = y * width + x;
			colorMap[4 * currentPixelIndex] = color.B;
			colorMap[4 * currentPixelIndex + 1] = color.G;
			colorMap[4 * currentPixelIndex + 2] = color.R;
			colorMap[4 * currentPixelIndex + 3] = color.A;
		}
	}
	return TextureFromColorMap(colorMap, width, height);
}