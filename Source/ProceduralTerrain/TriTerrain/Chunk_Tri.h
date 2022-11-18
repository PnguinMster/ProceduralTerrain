#pragma once

#include "CoreMinimal.h"
#include "TriMeshSettings.h"
#include "TriMapThreading.h"
#include "TriHeightMapSettings.h"
#include "ProceduralMeshComponent.h"
#include "Chunk_Tri.generated.h"

DECLARE_DELEGATE(FVoidDelegate);

UCLASS()
class PROCEDURALTERRAIN_API AChunk_Tri : public AActor
{
	GENERATED_BODY()
	
public:	
	AChunk_Tri();

	UProceduralMeshComponent* meshObject;
	UTriHeightMapSettings* heightMapSettings;
	UTriMeshSettings* meshSettings;
	UTri_HeightMap* mapData;
	ATriMapThreading* mapThread;

	FDataRecieved DataRecievedDelegate;
	FVoidDelegate UpdateChunkDelegate;

	FVector2D coord;
	FVector2D sampleCenter;
	FVector2D position;
	FVector2D* viewerPosition;

	bool heightMapRecieved;
	int previousLODIndex = -1;
	float maxViewDist;
	float meshWorldSize;

	TArray<UTriLODMesh*> lodMeshes;
	TArray<AChunk_Tri*>* visibleTerrainChunks;

	void Initialize(FVector2D Coord, float MeshWorldSize, UTriMeshSettings* MeshSettings, UTriHeightMapSettings* HeightMapSettings, ATriMapThreading* MapThread, float MaxViewDist, FVector2D& ViewerPosition, TArray<AChunk_Tri*>* VisibleTerrainChunks);
	void UpdateTerrainChunk();

	void OnHeightMapRecieved(UObject* heightMapObject);
	void SetTexture();
	
	static UTexture2D* ColorArrayToTexture(TArray<FColor> colors);
	static UTexture2D* FloatArrayToTexture(TArray<float> convertArray);

	void SetVisible(bool visible) { meshObject->SetVisibility(visible); }
	bool IsVisible() { return meshObject->IsVisible(); }
};

UCLASS()
class PROCEDURALTERRAIN_API UTriLODMesh : public UObject
{
	GENERATED_BODY()
public:
	FDataRecieved meshDataRecieved;
	FVoidDelegate* updateCallback;

	UProceduralMeshComponent* mesh;
	ATriMapThreading* mapThread;
	
	bool hasRequestedmesh;
	bool hasMesh;
	int lod;

	UTriLODMesh();
	void Initialize(int Lod, FVoidDelegate* UpdateCallback, ATriMapThreading* MapThread);
	void OnMeshDataRecieved(UObject* meshDataObject);
	void RequestMesh(UTri_HeightMap* heightMap, UTriMeshSettings* meshSettings);
};