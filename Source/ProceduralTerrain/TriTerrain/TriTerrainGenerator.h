#pragma once

#include "CoreMinimal.h"
#include "Chunk_Tri.h"
#include "HAL/Runnable.h"
#include "TriMapThreading.h"
#include "TriMeshGenerator.h"
#include "HAL/RunnableThread.h"
#include "GameFramework/Actor.h"
#include "TriTerrainGenerator.generated.h"

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
	TMap<FVector2D, AChunk_Tri*> terrainChunkMap;
	TArray<AChunk_Tri*> visibleTerrainChunks;

	APawn* ViewerPawn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UpdateVisibleChunk();
};