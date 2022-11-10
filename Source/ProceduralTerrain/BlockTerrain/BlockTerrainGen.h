#pragma once

#include "CoreMinimal.h"
#include "BlockChunk.h"
#include "BlockMapThreading.h"
#include "BlockTerrainGen.generated.h"

UCLASS()
class PROCEDURALTERRAIN_API ABlockTerrainGen : public AActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ABlockTerrainGen();

	UPROPERTY(EditAnywhere)
		bool CreateEndlessTerrain = true;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ABlockMapThreading> MapGenBP;
	UPROPERTY(EditAnywhere)
		TArray<FLODInfo> detailLevels;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UpdateVisibleChunks();

	//Viewing Variables
	const float ViewerMoveThresholdForChunkUpdate = 25.f;
	const float sqrViewerMoveThresholdForChunkUpdate = ViewerMoveThresholdForChunkUpdate * ViewerMoveThresholdForChunkUpdate;

	ABlockMapThreading* mapGen;
	FVector2D viewerPositionOld;
	FVector2D viewerPosition;
	APawn* followTarget;

	int chunksVisibleInViewDist = 8;
	float maxViewDist = 960;
	int chunkSize = 8;

	//Chunk Info Variables
	UPROPERTY()
		TArray<ABlockChunk*> visibleTerrainChunks;
	UPROPERTY()
		TMap<FVector2D, ABlockChunk*> chunkMap;

};
