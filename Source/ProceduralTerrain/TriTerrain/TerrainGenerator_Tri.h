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
		UMeshSettings_Tri* meshSettings;
	UPROPERTY(EditDefaultsOnly)
		UHeightMapSettings_Tri* heightMapSettings;

protected:
	virtual void BeginPlay() override;

	void UpdateVisibleChunk();
	void CreateChunk(FVector2D& viewedChunkCoord);

	const float moveThresholdForChunkUpdate = 300.f;
	int chunksVisibleInView;

	TMap<FVector2D, AChunk_Tri*> chunkMap;
	TArray<AChunk_Tri*> visibleChunks;

	UMapThreading_Tri* mapThread;
	APawn* ViewerPawn;

	FVector2D viewerPosition;
	FVector2D viewerPositionOld;
public:
	virtual void Tick(float DeltaTime) override;
};