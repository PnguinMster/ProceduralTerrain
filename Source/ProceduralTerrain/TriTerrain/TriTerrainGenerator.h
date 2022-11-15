#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "TriMapThreading.h"
#include "TriMeshGenerator.h"
#include "HAL/RunnableThread.h"
#include "GameFramework/Actor.h"
#include "TriTerrainGenerator.generated.h"

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
class PROCEDURALTERRAIN_API ATriTerrainGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATriTerrainGenerator();

	const float viewerMoveThresholdForChunkUpdate = 15.f;
	const float sqrViewerMoveThresholdForChunkUpdate = viewerMoveThresholdForChunkUpdate * viewerMoveThresholdForChunkUpdate;
	float maxViewDist;

	UPROPERTY(EditDefaultsOnly)
		UMaterialInterface* materialInterface;
	UPROPERTY(EditDefaultsOnly)
		TArray<FTriLODInfo> detailLevels;
	UPROPERTY(EditAnywhere)
		ATriMapThreading* mapThread;

	FVector2D viewerPosition;
	FVector2D viewerPositionOld;
	float meshWorldSize;
	int chunksVisibleInViewDist;
	TMap<FVector2D, ATriChunk*> terrainChunkMap;
	TArray<ATriChunk*> visibleTerrainChunks;

	APawn* ViewerPawn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UpdateVisibleChunk();
};

UCLASS()
class PROCEDURALTERRAIN_API ATriChunk : public AActor
{
	GENERATED_BODY()

public:
	FVector2D coord; 
	UProceduralMeshComponent* meshObject;
	ATriTerrainGenerator* terrainGen;
	FVector2D sampleCenter;
	FVector2D position;
	FMapReceived MapRecievedDelegate;
	FVoidDelegate UpdateChunkDelegate;

	TArray<UTriLODMesh*> lodMeshes;
	FTri_HeightMap mapData;
	bool mapDataRecieved;
	int previousLODIndex = -1;

public:
	// Sets default values for this actor's properties
	ATriChunk();

	void Initialize(FVector2D Coord, float MeshWorldSize, ATriTerrainGenerator* actor);
	void OnMapDataRecieved(FTri_HeightMap MapData);
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
	FMeshReceived meshDataRecieved;
	UProceduralMeshComponent* mesh;
	ATriMapThreading* mapThread;
	bool hasRequestedmesh;
	bool hasMesh;
	int lod;
	FVoidDelegate* updateCallback;

	UTriLODMesh();
	void Initialize(int Lod, FVoidDelegate* UpdateCallback, ATriMapThreading* MapThread);
	void OnMeshDataRecieved(UTriMeshData* meshData);
	void RequestMesh(FTri_HeightMap mapData);
};