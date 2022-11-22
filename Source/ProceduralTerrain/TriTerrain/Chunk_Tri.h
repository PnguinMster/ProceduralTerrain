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
	FVector2D coord;

	bool heightMapRecieved;

	void Initialize(UMapThreading_Tri* MapThread, UTriMeshSettings* MeshSettings, UTriHeightMapSettings* HeightMapSettings, TArray<AChunk_Tri*>* VisibleTerrainChunks, FVector2D Coord, FVector2D& ViewerPosition);
	void UpdateChunk();
	void OnHeightMapRecieved(UObject* heightMapObject);

protected:
	UMapThreading_Tri* mapThread;
	UProceduralMeshComponent* meshObject;
	UTri_HeightMap* mapData;
	UTriHeightMapSettings* heightMapSettings;
	UTriMeshSettings* meshSettings;

	FVector2D position;
	FVector2D sampleCenter;
	FVector2D* viewerPosition;

	TArray<AChunk_Tri*>* visibleTerrainChunks;
	TArray<ULODMesh_Tri*> lodMeshes;

	int previousLODIndex = -1;

	
	bool IsVisible() { return meshObject->IsVisible(); }

	static UTexture2D* ColorArrayToTexture(TArray<FColor> colors);
	static UTexture2D* FloatArrayToTexture(TArray<float> convertArray);

	void MakeMeshVisible(float viewerDistFromNearestChunk, bool shouldBeVisisble);
	void SetLODMeshes(TArray<FTriLODInfo>& detailLevels);
	void SetTexture();
	void SetVisible(bool visible) { meshObject->SetVisibility(visible); }
};