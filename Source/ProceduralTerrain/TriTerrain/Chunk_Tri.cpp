#include "Chunk_Tri.h"
#include "HeightMapGenerator_Tri.h"

AChunk_Tri::AChunk_Tri()
{
	meshObject = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	RootComponent = meshObject;
	UpdateChunkDelegate.BindUObject(this, &AChunk_Tri::UpdateTerrainChunk);
}

void AChunk_Tri::Initialize(FVector2D Coord, float MeshWorldSize, ATriMapThreading* actor, TArray<FTriLODInfo>* DetailLevels, float MaxViewDist, UMaterialInterface* MaterialInterface, FVector2D* ViewerPosition, TArray<AChunk_Tri*>* VisibleTerrainChunks)
{
	//DO NOT put in constructor
	//It Will Break
	DataRecievedDelegate.BindUObject(this, &AChunk_Tri::OnHeightMapRecieved);

	mapThread = actor;
	sampleCenter = Coord * MeshWorldSize / mapThread->meshSettings->meshScale;
	coord = Coord;
	FVector positionV3 = FVector(Coord.X, Coord.Y, 0) * MeshWorldSize;
	position = Coord * MeshWorldSize;
	maxViewDist = MaxViewDist;
	materialInterface = MaterialInterface;
	viewerPosition = ViewerPosition;
	visibleTerrainChunks = VisibleTerrainChunks;
	detailLevels = DetailLevels;

	meshObject->SetWorldLocation(positionV3);
	SetVisible(false);

	lodMeshes.SetNum((*detailLevels).Num());
	for (int i = 0; i < (*detailLevels).Num(); i++) {
		lodMeshes[i] = NewObject<UTriLODMesh>();
		lodMeshes[i]->Initialize((*detailLevels)[i].lod, &UpdateChunkDelegate, mapThread);
	}

	SetTexture();

	TFunction<UObject* (void)> function = [=]() {return HeightMapGenerator_Tri::GenerateHeightMap(mapThread->meshSettings->numVertsPerLine, mapThread->meshSettings->numVertsPerLine, mapThread->heightMapSettings, sampleCenter); };
	mapThread->RequestData(function, &DataRecievedDelegate);
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

void AChunk_Tri::SetTexture()
{
	//Add Texture to Material
	int regionsCount = mapThread->regions.Num();
	TArray<FColor> colors;
	TArray<float> heights;
	TArray<float> blends;
	colors.SetNum(regionsCount);
	heights.SetNum(regionsCount);
	blends.SetNum(regionsCount);
	for (int i = 0; i < regionsCount; i++) {
		colors[i] = mapThread->regions[i].color;
		heights[i] = mapThread->regions[i].height;
		blends[i] = mapThread->regions[i].blend;
	}

	UTexture2D* colorTexture = ColorArrayToTexture(colors);
	UTexture2D* heightTexture = FloatArrayToTexture(heights);
	UTexture2D* blendTexture = FloatArrayToTexture(blends);

	UMaterialInstanceDynamic* dynamicMaterialInstance = UMaterialInstanceDynamic::Create(materialInterface, meshObject->GetOwner());
	dynamicMaterialInstance->SetScalarParameterValue("MinHeight", mapThread->heightMapSettings->GetMinHeight());
	dynamicMaterialInstance->SetScalarParameterValue("MaxHeight", mapThread->heightMapSettings->GetMaxHeight());
	dynamicMaterialInstance->SetScalarParameterValue("RegionCount", regionsCount);
	dynamicMaterialInstance->SetTextureParameterValue("ColorTexture", colorTexture);
	dynamicMaterialInstance->SetTextureParameterValue("HeightTexture", heightTexture);
	dynamicMaterialInstance->SetTextureParameterValue("BlendTexture", blendTexture);

	meshObject->SetMaterial(0, dynamicMaterialInstance);
}

void AChunk_Tri::OnHeightMapRecieved(UObject* heightMapObject)
{
	mapData = Cast<UTri_HeightMap>(heightMapObject);
	heightMapRecieved = true;

	UpdateTerrainChunk();
}

void AChunk_Tri::UpdateTerrainChunk()
{
	if (heightMapRecieved) 
	{
		float viewerDistFromNearestChunk = FVector2D::Distance(position, *viewerPosition);
		bool wasVisible = IsVisible();
		bool visible = viewerDistFromNearestChunk <= maxViewDist;

		if (visible) {
			int lodIndex = 0;

			for (int i = 0; i < (*detailLevels).Num() - 1; i++) {
				if (viewerDistFromNearestChunk > (*detailLevels)[i].visibleChunks * meshWorldSize)
					lodIndex = i + 1;
				else 
					break;
			}

			if (lodIndex != previousLODIndex) {
				UE_LOG(LogTemp, Warning, TEXT("The integer value is: %d"), lodIndex);
				UTriLODMesh* lodMesh = lodMeshes[lodIndex];
				if (lodMesh->hasMesh) {
					previousLODIndex = lodIndex;
					meshObject->SetProcMeshSection(0, *lodMesh->mesh->GetProcMeshSection(0));
				}
				else if (!lodMesh->hasRequestedmesh) {
					lodMesh->RequestMesh(mapData, mapThread->meshSettings);
				}
			}
		}

		if (wasVisible != visible) {
			if (visible)
				visibleTerrainChunks->Add(this);
			else 
				visibleTerrainChunks->Remove(this);

			SetVisible(visible);
		}
	}
}

UTriLODMesh::UTriLODMesh()
{
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
}

void UTriLODMesh::Initialize(int Lod, FVoidDelegate* UpdateCallback, ATriMapThreading* MapThread)
{
	meshDataRecieved.BindUObject(this, &UTriLODMesh::OnMeshDataRecieved);
	lod = Lod;
	mapThread = MapThread;
	updateCallback = UpdateCallback;
}

void UTriLODMesh::OnMeshDataRecieved(UObject* meshDataObject)
{
	Cast<UTriMeshData>(meshDataObject)->CreateMesh(mesh);
	hasMesh = true;

	updateCallback->Execute();
}

void UTriLODMesh::RequestMesh(UTri_HeightMap* heightMap, UTriMeshSettings* meshSettings)
{
	hasRequestedmesh = true;
	TFunction<UObject* (void)> function = [=]() { return TriMeshGenerator::GenerateTerrainMesh(heightMap->values, meshSettings, lod); };

	mapThread->RequestData(function, &meshDataRecieved);
}