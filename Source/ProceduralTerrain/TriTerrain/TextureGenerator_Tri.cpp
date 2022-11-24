#include "TextureGenerator_Tri.h"
#include "Kismet/KismetMathLibrary.h"

UMaterialInstanceDynamic* TextureGenerator_Tri::CreateMaterialInstance(UMeshSettings_Tri* meshSettings, UHeightMapSettings_Tri* heightMapSettings, UObject* inOuter)
{
	int regionsCount = heightMapSettings->regions.Num();

	TArray<FColor> colors;
	TArray<float> heights;
	TArray<float> blends;

	colors.SetNum(regionsCount);
	heights.SetNum(regionsCount);
	blends.SetNum(regionsCount);

	for (int i = 0; i < regionsCount; i++) {
		colors[i] = heightMapSettings->regions[i].color;
		heights[i] = heightMapSettings->regions[i].height;
		blends[i] = heightMapSettings->regions[i].blend;
	}

	UTexture2D* colorTexture = ColorArrayToTexture(colors);
	UTexture2D* heightTexture = FloatArrayToTexture(heights);
	UTexture2D* blendTexture = FloatArrayToTexture(blends);

	UMaterialInstanceDynamic* dynamicMaterialInstance = UMaterialInstanceDynamic::Create(meshSettings->materialInterface, inOuter);
	dynamicMaterialInstance->SetScalarParameterValue("MinHeight", heightMapSettings->GetMinHeight());
	dynamicMaterialInstance->SetScalarParameterValue("MaxHeight", heightMapSettings->GetMaxHeight());
	dynamicMaterialInstance->SetScalarParameterValue("RegionCount", regionsCount);
	dynamicMaterialInstance->SetTextureParameterValue("ColorTexture", colorTexture);
	dynamicMaterialInstance->SetTextureParameterValue("HeightTexture", heightTexture);
	dynamicMaterialInstance->SetTextureParameterValue("BlendTexture", blendTexture);
	return dynamicMaterialInstance;
}

UTexture2D* TextureGenerator_Tri::TextureFromColorMap(TArray<uint8> colorMap, int width, int height)
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

UTexture2D* TextureGenerator_Tri::TextureFromHeightMap(TArray<TArray<float>> heightMap)
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

UTexture2D* TextureGenerator_Tri::ColorArrayToTexture(TArray<FColor> colors)
{
	UTexture2D* texture = UTexture2D::CreateTransient(colors.Num(), 1, PF_B8G8R8A8);

	if (!texture) return nullptr;
#if WITH_EDITORONLY_DATA
	texture->MipGenSettings = TMGS_NoMipmaps;
#endif
	texture->NeverStream = true;
	texture->SRGB = 0;
	texture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
	FTexture2DMipMap& Mip = texture->GetPlatformData()->Mips[0];
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(Data, colors.GetData(), colors.Num() * 4);
	Mip.BulkData.Unlock();
	texture->UpdateResource();
	return texture;
}

UTexture2D* TextureGenerator_Tri::FloatArrayToTexture(TArray<float> convertArray)
{
	UTexture2D* texture = UTexture2D::CreateTransient(convertArray.Num(), 1, PF_R32_FLOAT);

	if (!texture) return nullptr;
#if WITH_EDITORONLY_DATA
	texture->MipGenSettings = TMGS_NoMipmaps;
#endif
	texture->NeverStream = true;
	texture->SRGB = 0;
	texture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
	FTexture2DMipMap& Mip = texture->GetPlatformData()->Mips[0];
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(Data, convertArray.GetData(), convertArray.Num() * 4);
	Mip.BulkData.Unlock();
	texture->UpdateResource();
	return texture;
}