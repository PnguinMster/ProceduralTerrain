#include "Chunk_Tri.h"
#include "TextureGenerator_Tri.h"
#include "HeightMapGenerator_Tri.h"

AChunk_Tri::AChunk_Tri()
{
	meshObject = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	RootComponent = meshObject;
	UpdateChunkDelegate.BindUObject(this, &AChunk_Tri::UpdateChunk);
}

void AChunk_Tri::Initialize(UMapThreading_Tri* MapThread, UMeshSettings_Tri* MeshSettings, UHeightMapSettings_Tri* HeightMapSettings, TArray<AChunk_Tri*>* VisibleTerrainChunks, FVector2D Coord, FVector2D& ViewerPosition)
{
	DataRecievedDelegate.BindUObject(this, &AChunk_Tri::OnHeightMapRecieved);

	mapThread = MapThread;
	meshSettings = MeshSettings;
	heightMapSettings = HeightMapSettings;
	coord = Coord;
	visibleTerrainChunks = VisibleTerrainChunks;
	viewerPosition = &ViewerPosition;

	FVector2D chunkPosition = Coord * meshSettings->GetMeshWorldSize();
	sampleCenter = chunkPosition / meshSettings->meshScale;

	SetLODMeshes(meshSettings->detailLevels);
	SetVisible(false);

	meshObject->SetMaterial(0, TextureGenerator_Tri::CreateMaterialInstance(meshSettings, heightMapSettings, this));
	meshObject->SetWorldLocation(FVector(chunkPosition.X, chunkPosition.Y, 0));

	TFunction<UObject* (void)> function = [=]() {return HeightMapGenerator_Tri::GenerateHeightMap(meshSettings->numberOfVerticesPerLine, meshSettings->numberOfVerticesPerLine, heightMapSettings, sampleCenter); };
	mapThread->RequestData(function, &DataRecievedDelegate);
}

void AChunk_Tri::UpdateChunk()
{
	if (!heightMapRecieved)
		return;

	float viewerDistFromNearestChunk = FVector2D::Distance(FVector2D(GetActorLocation().X, GetActorLocation().Y), *viewerPosition);
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
	mapData = Cast<UHeightMap_Tri>(heightMapObject);
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

void AChunk_Tri::SetLODMeshes(TArray<FLODInfo_Tri>& detailLevels)
{
	lodMeshes.SetNum(detailLevels.Num());
	for (int i = 0; i < detailLevels.Num(); i++) {
		lodMeshes[i] = NewObject<ULODMesh_Tri>();
		lodMeshes[i]->Initialize(detailLevels[i].lod, &UpdateChunkDelegate);
	}
}