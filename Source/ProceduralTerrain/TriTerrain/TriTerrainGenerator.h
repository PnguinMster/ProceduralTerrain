#pragma once

#include "CoreMinimal.h"
#include "Chunk_Tri.h"
#include "TriMapThreading.h"
#include "TriMeshGenerator.h"
#include "TriMeshSettings.h"
#include "TriHeightMapSettings.h"
#include "TriTerrainGenerator.generated.h"

UCLASS()
class PROCEDURALTERRAIN_API ATriTerrainGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	ATriTerrainGenerator();

	UPROPERTY(EditDefaultsOnly)
		UTriMeshSettings* meshSettings;
	UPROPERTY(EditDefaultsOnly)
		UTriHeightMapSettings* heightMapSettings;
protected:
	virtual void BeginPlay() override;

	const float viewerMoveThresholdForChunkUpdate = 15.f;
	const float sqrViewerMoveThresholdForChunkUpdate = viewerMoveThresholdForChunkUpdate * viewerMoveThresholdForChunkUpdate;
	int chunksVisibleInViewDist;

	TMap<FVector2D, AChunk_Tri*> terrainChunkMap;
	TArray<AChunk_Tri*> visibleTerrainChunks;

	UTriMapThreading* mapThread;
	APawn* ViewerPawn;
	FVector2D viewerPosition;
	FVector2D viewerPositionOld;
public:	
	virtual void Tick(float DeltaTime) override;

	void UpdateVisibleChunk();
};