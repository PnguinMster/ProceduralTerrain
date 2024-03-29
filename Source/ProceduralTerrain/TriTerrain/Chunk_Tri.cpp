#include "Chunk_Tri.h"
#include "TextureGenerator_Tri.h"
#include "HeightMapGenerator_Tri.h"

AChunk_Tri::AChunk_Tri()
{
	MeshObject = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshObject;
}

void AChunk_Tri::Initialize(UMapThreading* mapThread, UMeshSettings_Tri* meshSettings, UHeightMapSettings_Tri* heightMapSettings, TArray<AChunk_Tri*>* visibleTerrainChunks, FVector2D coord, FVector2D& viewerPosition)
{
	UpdateChunkDelegate.BindUObject(this, &AChunk_Tri::UpdateChunk);
	DataRecievedDelegate.BindUObject(this, &AChunk_Tri::OnHeightMapRecieved);

	MapThread = mapThread;
	MeshSettings = meshSettings;
	HeightMapSettings = heightMapSettings;
	Coord = coord;
	VisibleTerrainChunks = visibleTerrainChunks;
	ViewerPosition = &viewerPosition;

	SetLODMeshes(meshSettings->DetailLevels);
	SetVisible(false);

	MeshObject->SetWorldLocation(FVector(ChunkPosition().X, ChunkPosition().Y, 0));

	TFunction<UObject* (void)> function = [=]() {return HeightMapGenerator_Tri::GenerateHeightMap(meshSettings->GetNumberVerticesPerLine(), meshSettings->GetNumberVerticesPerLine(), heightMapSettings, SampleCenter(), meshSettings->MeshScale); };
	mapThread->RequestData(function, &DataRecievedDelegate);
}

void AChunk_Tri::UpdateChunk()
{
	if (!HeightMapRecieved)
		return;

	float maxViewDist = MeshSettings->DetailLevels[MeshSettings->DetailLevels.Num() - 1].VisibleChunks * MeshSettings->GetMeshWorldSize();
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

void AChunk_Tri::OnHeightMapRecieved(UObject* heightMapObject)
{
	HeightMap = Cast<UHeightMap_Tri>(heightMapObject);
	HeightMapRecieved = true;
	MeshObject->SetMaterial(0, TextureGenerator_Tri::CreateMaterialInstance(MeshSettings, HeightMapSettings, this, HeightMap));

	UpdateChunk();
}

void AChunk_Tri::MakeMeshVisible(float viewerDistFromNearestChunk, bool shouldBeVisible)
{
	if (!shouldBeVisible)
		return;

	int lodIndex = 0;
	for (int i = 0; i < MeshSettings->DetailLevels.Num() - 1; i++) {
		if (viewerDistFromNearestChunk > MeshSettings->DetailLevels[i].VisibleChunks * MeshSettings->GetMeshWorldSize())
			lodIndex = i + 1;
		else
			break;
	}

	if (lodIndex != PreviousLODIndex) {
		ULODMesh_Tri* lodMesh = LodMeshes[lodIndex];
		if (lodMesh->HasMesh) {
			PreviousLODIndex = lodIndex;
			MeshObject->SetProcMeshSection(0, *lodMesh->Mesh->GetProcMeshSection(0));
		}
		else if (!lodMesh->HasRequestedMesh)
			lodMesh->RequestMesh(HeightMap, MeshSettings, MapThread);
	}
}

void AChunk_Tri::SetLODMeshes(TArray<FLODInfo_Tri>& detailLevels)
{
	LodMeshes.SetNum(detailLevels.Num());
	for (int i = 0; i < detailLevels.Num(); i++) {
		LodMeshes[i] = NewObject<ULODMesh_Tri>();
		LodMeshes[i]->Initialize(detailLevels[i].Lod, &UpdateChunkDelegate);
	}
}

FVector2D AChunk_Tri::ChunkPosition()
{
	return Coord * MeshSettings->GetMeshWorldSize();
}

FVector2D AChunk_Tri::SampleCenter()
{
	return ChunkPosition() / MeshSettings->MeshScale;
}
