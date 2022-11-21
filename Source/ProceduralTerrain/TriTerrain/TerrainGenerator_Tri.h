#pragma once

#include "CoreMinimal.h"
#include "Chunk_Tri.h"
#include "TriMapThreading.h"
#include "TriMeshGenerator.h"
#include "TriMeshSettings.h"
#include "TriHeightMapSettings.h"
#include "TerrainGenerator_Tri.generated.h"

UCLASS()
class PROCEDURALTERRAIN_API ATerrainGenerator_Tri : public AActor
{
	GENERATED_BODY()
	
public:	
	ATerrainGenerator_Tri();

	UPROPERTY(EditDefaultsOnly)
		UTriMeshSettings* meshSettings;
	UPROPERTY(EditDefaultsOnly)
		UTriHeightMapSettings* heightMapSettings;
protected:
	virtual void BeginPlay() override;

	void UpdateVisibleChunk();
	void CreateChunk(FVector2D& viewedChunkCoord);

	const float moveThresholdForChunkUpdate = 300.f;
	int chunksVisibleInView;

	TMap<FVector2D, AChunk_Tri*> chunkMap;
	TArray<AChunk_Tri*> visibleChunks;

	UTriMapThreading* mapThread;
	APawn* ViewerPawn;

	FVector2D viewerPosition;
	FVector2D viewerPositionOld;
public:	
	virtual void Tick(float DeltaTime) override;

};