#pragma once

#include "CoreMinimal.h"
#include "LODMesh_Tri.h"
#include "Chunk_Tri.generated.h"

UCLASS()
class PROCEDURALTERRAIN_API AChunk_Tri : public AActor
{
	GENERATED_BODY()

public:
	AChunk_Tri();

	FDataRecieved DataRecievedDelegate;
	FVoidDelegate UpdateChunkDelegate;
	FVector2D Coord;

	bool HeightMapRecieved;

	void Initialize(UMapThreading* mapThread, UMeshSettings_Tri* meshSettings, UHeightMapSettings_Tri* heightMapSettings, TArray<AChunk_Tri*>* visibleTerrainChunks, FVector2D coord, FVector2D& viewerPosition);
	void UpdateChunk();
	void OnHeightMapRecieved(UObject* heightMapObject);

protected:
	UMapThreading* MapThread;
	UProceduralMeshComponent* MeshObject;
	UHeightMap_Tri* HeightMap;
	UHeightMapSettings_Tri* HeightMapSettings;
	UMeshSettings_Tri* MeshSettings;

	FVector2D* ViewerPosition;

	TArray<AChunk_Tri*>* VisibleTerrainChunks;
	TArray<ULODMesh_Tri*> LodMeshes;

	int PreviousLODIndex = -1;
	
	bool IsVisible() { return MeshObject->IsVisible(); }

	void MakeMeshVisible(float viewerDistFromNearestChunk, bool shouldBeVisisble);
	void SetLODMeshes(TArray<FLODInfo_Tri>& detailLevels);
	void SetVisible(bool visible) { MeshObject->SetVisibility(visible); }
	FVector2D ChunkPosition();
	FVector2D SampleCenter();
};