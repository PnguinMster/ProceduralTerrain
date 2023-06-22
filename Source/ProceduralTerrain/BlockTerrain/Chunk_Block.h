#pragma once

#include "CoreMinimal.h"
#include "LODMesh_Block.h"
#include "Chunk_Block.generated.h"

UCLASS()
class PROCEDURALTERRAIN_API AChunk_Block : public AActor
{
	GENERATED_BODY()

public:
	AChunk_Block();

	FDataRecieved DataRecievedDelegate;
	FVoidDelegate UpdateChunkDelegate;
	FVector2D Coord;

	bool HeightMapRecieved;

	void Initialize(UMapThreading* mapThread, UMeshSettings_Block* meshSettings, UHeightMapSettings_Block* heightMapSettings, TArray<AChunk_Block*>* visibleTerrainChunks, FVector2D coord, FVector2D& viewerPosition);
	void UpdateChunk();
	void OnHeightMapRecieved(UObject* heightMapObject);

protected:
	UMapThreading* MapThread;
	UProceduralMeshComponent* MeshObject;
	UHeightMap_Block* HeightMap;
	UHeightMapSettings_Block* HeightMapSettings;
	UMeshSettings_Block* MeshSettings;

	FVector2D* ViewerPosition;

	TArray<AChunk_Block*>* VisibleTerrainChunks;
	TArray <ULODMesh_Block* > LodMeshes;

	int PreviousLODIndex = -1;

	bool IsVisible() { return MeshObject->IsVisible(); }

	void MakeMeshVisible(float viewerDistFromNearestChunk, bool shouldBeVisisble);
	void SetLODMeshes(TArray<FLODInfo_Block>& detailLevels);
	void SetVisible(bool visible) { MeshObject->SetVisibility(visible); }
	FVector2D ChunkPosition();
	FVector2D SampleCenter();
};
