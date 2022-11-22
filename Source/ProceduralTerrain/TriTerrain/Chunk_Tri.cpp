#include "Chunk_Tri.h"
#include "HeightMapGenerator_Tri.h"

AChunk_Tri::AChunk_Tri()
{
	meshObject = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	RootComponent = meshObject;
	UpdateChunkDelegate.BindUObject(this, &AChunk_Tri::UpdateChunk);
}

void AChunk_Tri::Initialize(UMapThreading_Tri* MapThread, UTriMeshSettings* MeshSettings, UTriHeightMapSettings* HeightMapSettings, TArray<AChunk_Tri*>* VisibleTerrainChunks, FVector2D Coord, FVector2D& ViewerPosition)
{
	DataRecievedDelegate.BindUObject(this, &AChunk_Tri::OnHeightMapRecieved);

	mapThread = MapThread;
	meshSettings = MeshSettings;
	heightMapSettings = HeightMapSettings;
	coord = Coord;
	visibleTerrainChunks = VisibleTerrainChunks;
	viewerPosition = &ViewerPosition;

	position = Coord * meshSettings->GetMeshWorldSize();
	sampleCenter = position / meshSettings->meshScale;

	SetLODMeshes(meshSettings->detailLevels);
	SetVisible(false);
	SetTexture();
	meshObject->SetWorldLocation(FVector(position.X, position.Y, 0));
	TFunction<UObject* (void)> function = [=]() {return HeightMapGenerator_Tri::GenerateHeightMap(meshSettings->numVertsPerLine, meshSettings->numVertsPerLine, heightMapSettings, sampleCenter); };

	mapThread->RequestData(function, &DataRecievedDelegate);
}

void AChunk_Tri::UpdateChunk()
{
	if (!heightMapRecieved)
		return;

	float viewerDistFromNearestChunk = FVector2D::Distance(position, *viewerPosition);
	bool shouldBeVisible = viewerDistFromNearestChunk <= meshSettings->detailLevels[meshSettings->detailLevels.Num() - 1].visibleChunks * meshSettings->GetMeshWorldSize();

	MakeMeshVisible(viewerDistFromNearestChunk, shouldBeVisible);

	if (IsVisible() != shouldBeVisible) {
		if (shouldBeVisible)
			visibleTerrainChunks->Add(this);
		else
			visibleTerrainChunks->Remove(this);
	}

	SetVisible(shouldBeVisible);
}

void AChunk_Tri::OnHeightMapRecieved(UObject* heightMapObject)
{
	mapData = Cast<UTri_HeightMap>(heightMapObject);
	heightMapRecieved = true;

	UpdateChunk();
}

void AChunk_Tri::MakeMeshVisible(float viewerDistFromNearestChunk, bool shouldBeVisible)
{
	if (!shouldBeVisible)
		return;

	int lodIndex = 0;
	for (int i = 0; i < meshSettings->detailLevels.Num() - 1; i++) {
		if (viewerDistFromNearestChunk > meshSettings->detailLevels[i].visibleChunks * meshSettings->GetMeshWorldSize())
			lodIndex = i + 1;
		else
			break;
	}

	if (lodIndex != previousLODIndex) {
		ULODMesh_Tri* lodMesh = lodMeshes[lodIndex];
		if (lodMesh->hasMesh) {
			previousLODIndex = lodIndex;
			meshObject->SetProcMeshSection(0, *lodMesh->mesh->GetProcMeshSection(0));
		}
		else if (!lodMesh->hasRequestedmesh)
			lodMesh->RequestMesh(mapData, meshSettings, mapThread);
	}
}

void AChunk_Tri::SetLODMeshes(TArray<FTriLODInfo>& detailLevels)
{
	lodMeshes.SetNum(detailLevels.Num());
	for (int i = 0; i < detailLevels.Num(); i++) {
		lodMeshes[i] = NewObject<ULODMesh_Tri>();
		lodMeshes[i]->Initialize(detailLevels[i].lod, &UpdateChunkDelegate);
	}
}

void AChunk_Tri::SetTexture()
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

	UMaterialInstanceDynamic* dynamicMaterialInstance = UMaterialInstanceDynamic::Create(meshSettings->materialInterface, meshObject->GetOwner());
	dynamicMaterialInstance->SetScalarParameterValue("MinHeight", heightMapSettings->GetMinHeight());
	dynamicMaterialInstance->SetScalarParameterValue("MaxHeight", heightMapSettings->GetMaxHeight());
	dynamicMaterialInstance->SetScalarParameterValue("RegionCount", regionsCount);
	dynamicMaterialInstance->SetTextureParameterValue("ColorTexture", colorTexture);
	dynamicMaterialInstance->SetTextureParameterValue("HeightTexture", heightTexture);
	dynamicMaterialInstance->SetTextureParameterValue("BlendTexture", blendTexture);

	meshObject->SetMaterial(0, dynamicMaterialInstance);
}

UTexture2D* AChunk_Tri::ColorArrayToTexture(TArray<FColor> colors)
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

UTexture2D* AChunk_Tri::FloatArrayToTexture(TArray<float> convertArray)
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