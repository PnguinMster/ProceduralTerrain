#include "Chunk_Block.h"
#include "TextureGenerator_Block.h"
#include "HeightMapGenerator_Block.h"

AChunk_Block::AChunk_Block()
{
	MeshObject = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshObject;
}

void AChunk_Block::Initialize(UMapThreading* mapThread, UMeshSettings_Block* meshSettings, UHeightMapSettings_Block* heightMapSettings, TArray<AChunk_Block*>* visibleTerrainChunks, FVector2D coord, FVector2D& viewerPosition)
{
	UpdateChunkDelegate.BindUObject(this, &AChunk_Block::UpdateChunk);
	DataRecievedDelegate.BindUObject(this, &AChunk_Block::OnHeightMapRecieved);

	MapThread = mapThread;
	MeshSettings = meshSettings;
	HeightMapSettings = heightMapSettings;
	Coord = coord;
	VisibleTerrainChunks = visibleTerrainChunks;
	ViewerPosition = &viewerPosition;

	SetLODMeshes(meshSettings->DetailLevels);
	SetVisible(false);

	MeshObject->SetWorldLocation(FVector(ChunkPosition().X, ChunkPosition().Y, 0));

	TFunction<UObject* (void)> function = [=]() {return HeightMapGenerator_Block::GenerateHeightMap(meshSettings->GetMeshVertsNum(), heightMapSettings, SampleCenter()); };
	mapThread->RequestData(function, &DataRecievedDelegate);
}

void AChunk_Block::UpdateChunk()
{
	if (!HeightMapRecieved)
		return;

	float maxViewDist = MeshSettings->DetailLevels[MeshSettings->DetailLevels.Num() - 1].ChunksVisible * MeshSettings->GetMeshWorldSize();
	float viewerDistFromNearestChunk = FVector2D::Distance(FVector2D(GetActorLocation().X, GetActorLocation().Y), *ViewerPosition);
	bool shouldBeVisible = viewerDistFromNearestChunk <= maxViewDist;
	MakeMeshVisible(viewerDistFromNearestChunk, shouldBeVisible);

	if (IsVisible() != shouldBeVisible) {
		if (shouldBeVisible)
			VisibleTerrainChunks->Add(this);
		else
			VisibleTerrainChunks->Remove(this);
	}

	SetVisible(shouldBeVisible);
}

void AChunk_Block::OnHeightMapRecieved(UObject* heightMapObject)
{
	HeightMap = Cast<UHeightMap_Block>(heightMapObject);
	HeightMapRecieved = true;
	MeshObject->SetMaterial(0, TextureGenerator_Block::CreateMaterialInstance(HeightMap, MeshSettings, this));

	UpdateChunk();
}

void AChunk_Block::MakeMeshVisible(float viewerDistFromNearestChunk, bool shouldBeVisible)
{
	if (!shouldBeVisible)
		return;

	int lodIndex = 0;
	for (int i = 0; i < MeshSettings->DetailLevels.Num() - 1; i++) {
		if (viewerDistFromNearestChunk > MeshSettings->DetailLevels[i].ChunksVisible * MeshSettings->GetMeshWorldSize())
			lodIndex = i + 1;
		else
			break;
	}

	if (lodIndex != PreviousLODIndex) {
		ULODMesh_Block* lodMesh = LodMeshes[lodIndex];
		lodMesh->Mesh->UpdateBounds();
		if (lodMesh->HasMesh) {
			PreviousLODIndex = lodIndex;
			MeshObject->SetProcMeshSection(0, *lodMesh->Mesh->GetProcMeshSection(0));
		}
		else if (!lodMesh->HasRequestedMesh)
			lodMesh->RequestMesh(HeightMap, MeshSettings, MapThread);
	}
}

void AChunk_Block::SetLODMeshes(TArray<FLODInfo_Block>& detailLevels)
{
	LodMeshes.SetNum(detailLevels.Num());
	for (int i = 0; i < detailLevels.Num(); i++) {
		LodMeshes[i] = NewObject<ULODMesh_Block>();
		LodMeshes[i]->Initialize(detailLevels[i].Lod, &UpdateChunkDelegate);
	}
}

FVector2D AChunk_Block::ChunkPosition()
{
	return Coord * MeshSettings->GetMeshWorldSize();
}

FVector2D AChunk_Block::SampleCenter()
{
	return (MeshSettings->GetMeshVertsNum() - 2) * Coord;
}