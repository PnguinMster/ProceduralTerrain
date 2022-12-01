#pragma once

#include "CoreMinimal.h"
#include "Chunk_Block.h"
#include "MapThreading_Block.h"
#include "HeightMapSettings_Block.h"
#include "MeshGenerator_Block.h"
#include "MeshSettings_Block.h"
#include "TerrainGenerator_Block.generated.h"

UCLASS()
class PROCEDURALTERRAIN_API ATerrainGenerator_Block : public AActor
{
	GENERATED_BODY()

public:
	ATerrainGenerator_Block();

	UPROPERTY(EditDefaultsOnly)
		UMeshSettings_Block* MeshSettings;
	UPROPERTY(EditDefaultsOnly)
		UHeightMapSettings_Block* HeightMapSettings;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	void UpdateVisibleChunks();
	void CreateChunk(FVector2D& viewedChunkCoord);

	const float MoveThresholdForChunkUpdate = 300.f;
	int ChunksVisibleInView;

	TMap<FVector2D, AChunk_Block*> ChunkMap;
	TArray<AChunk_Block*> VisibleChunks;

	UMapThreading_Block* MapThread;
	APawn* ViewerPawn;

	FVector2D ViewerPositionOld;
	FVector2D ViewerPosition;
};
