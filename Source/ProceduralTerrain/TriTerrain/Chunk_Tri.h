#pragma once

#include "CoreMinimal.h"
#include "TriMeshGenerator.h"
#include "TriMapThreading.h"
#include "GameFramework/Actor.h"
#include "Chunk_Tri.generated.h"

DECLARE_DELEGATE(FVoidDelegate);

USTRUCT()
struct FTriLODInfo
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0", UIMin = "1.0"))
		int visibleChunks;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "6.0", UIMin = "0.0", UIMax = "6.0"))
		int lod;
};

UCLASS()
class PROCEDURALTERRAIN_API AChunk_Tri : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunk_Tri();

	UPROPERTY(VisibleAnywhere)
	FVector2D coord;
	UProceduralMeshComponent* meshObject;
	ATriMapThreading* mapThread;
	UPROPERTY(VisibleAnywhere)
	FVector2D sampleCenter;
	UPROPERTY(VisibleAnywhere)
	FVector2D position;
	FDataRecieved DataRecievedDelegate;
	FVoidDelegate UpdateChunkDelegate;

	TArray<UTriLODMesh*> lodMeshes;
	UTri_HeightMap* mapData;
	bool heightMapRecieved;
	UPROPERTY(VisibleAnywhere)
	int previousLODIndex = -1;
	float maxViewDist;
	FVector2D* viewerPosition;
	float meshWorldSize;
	TArray<FTriLODInfo>* detailLevels;
	UMaterialInterface* materialInterface;
	TArray<AChunk_Tri*>* visibleTerrainChunks;

	void Initialize(FVector2D Coord, float MeshWorldSize, ATriMapThreading* actor, TArray<FTriLODInfo>& DetailLevels, float MaxViewDist, UMaterialInterface* MaterialInterface, FVector2D& ViewerPosition, TArray<AChunk_Tri*>* VisibleTerrainChunks);
	void OnHeightMapRecieved(UObject* heightMapObject);
	void SetTexture();
	static UTexture2D* ColorArrayToTexture(TArray<FColor> colors);
	static UTexture2D* FloatArrayToTexture(TArray<float> convertArray);

	void UpdateTerrainChunk();

	void SetVisible(bool visible) { meshObject->SetVisibility(visible); }
	bool IsVisible() { return meshObject->IsVisible(); }

};

UCLASS()
class PROCEDURALTERRAIN_API UTriLODMesh : public UObject
{
	GENERATED_BODY()
public:
	FDataRecieved meshDataRecieved;
	UProceduralMeshComponent* mesh;
	ATriMapThreading* mapThread;
	bool hasRequestedmesh;
	bool hasMesh;
	UPROPERTY(VisibleAnywhere)
	int lod;
	FVoidDelegate* updateCallback;

	UTriLODMesh();
	void Initialize(int Lod, FVoidDelegate* UpdateCallback, ATriMapThreading* MapThread);
	void OnMeshDataRecieved(UObject* meshDataObject);
	void RequestMesh(UTri_HeightMap* heightMap, UTriMeshSettings* meshSettings);
};