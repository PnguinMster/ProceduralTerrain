#pragma once

#include "CoreMinimal.h"
#include "Chunk_Tri.h"
#include "MapThreading_Tri.h"
#include "HeightMapSettings_Tri.h"
#include "MeshGenerator_Tri.h"
#include "MeshSettings_Tri.h"
#include "TerrainGenerator_Tri.generated.h"

UCLASS()
class PROCEDURALTERRAIN_API ATerrainGenerator_Tri : public AActor
{
	GENERATED_BODY()

public:
	ATerrainGenerator_Tri();

	UPROPERTY(EditDefaultsOnly)
		UMeshSettings_Tri* MeshSettings;
	UPROPERTY(EditDefaultsOnly)
		UHeightMapSettings_Tri* HeightMapSettings;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	void UpdateVisibleChunk();
	void CreateChunk(FVector2D& viewedChunkCoord);

	const float MoveThresholdForChunkUpdate = 300.f;
	int ChunksVisibleInView;

	TMap<FVector2D, AChunk_Tri*> ChunkMap;
	TArray<AChunk_Tri*> VisibleChunks;

	UMapThreading_Tri* MapThread;
	APawn* ViewerPawn;

	FVector2D ViewerPosition;
	FVector2D ViewerPositionOld;
};