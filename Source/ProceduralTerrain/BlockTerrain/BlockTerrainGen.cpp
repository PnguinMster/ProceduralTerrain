#include "BlockTerrainGen.h"

ABlockTerrainGen::ABlockTerrainGen()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABlockTerrainGen::BeginPlay()
{
	Super::BeginPlay();

	followTarget = GetWorld()->GetFirstPlayerController()->GetPawn();

	//Spawn Map Generator
	FVector Location(0.0f, 0.0f, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;
	mapGen = GetWorld()->SpawnActor<ABlockMapThreading>(MapGenBP, Location, Rotation, SpawnInfo);

	//Calculate chunk Size
	chunkSize = (mapGen->meshSettings->chunkSize * mapGen->meshSettings->BlockSize) * 2.f;
	chunksVisibleInViewDist = CreateEndlessTerrain ? detailLevels[detailLevels.Num() - 1].chunksVisible : 1;
	maxViewDist = chunksVisibleInViewDist * chunkSize;

	UpdateVisibleChunks();
}

void ABlockTerrainGen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CreateEndlessTerrain)
		return;

	//Get Players Location
	FVector location = followTarget->GetActorLocation();
	viewerPosition = FVector2D(location.X, location.Y);

	//Check If Viewer Moved
	if ((viewerPositionOld - viewerPosition).SizeSquared() > sqrViewerMoveThresholdForChunkUpdate) {
		viewerPositionOld = viewerPosition;
		UpdateVisibleChunks();
	}
}

void ABlockTerrainGen::UpdateVisibleChunks()
{
	TSet<FVector2D> alreadyUpdatedChunkCoord;
	//Disable Old Chunks
	for (int i = visibleTerrainChunks.Num() - 1; i >= 0; i--) {
		alreadyUpdatedChunkCoord.Add(visibleTerrainChunks[i]->GetCoord());
		visibleTerrainChunks[i]->UpdateChunk();
	}

	//Chunk Coord
	int currentChunkCoordX = FMath::RoundToInt(viewerPosition.X / chunkSize);
	int currentChunkCoordY = FMath::RoundToInt(viewerPosition.Y / chunkSize);

	//Update Chunk Visibility
	for (int yOffset = -chunksVisibleInViewDist; yOffset <= chunksVisibleInViewDist; yOffset++) {
		for (int xOffset = -chunksVisibleInViewDist; xOffset <= chunksVisibleInViewDist; xOffset++) {
			FVector2D viewedChunkCoord = FVector2D(currentChunkCoordX + xOffset, currentChunkCoordY + yOffset);

			if (!alreadyUpdatedChunkCoord.Contains(viewedChunkCoord)) {
				//Update Current Chunk
				if (chunkMap.Contains(viewedChunkCoord)) {
					//Update Chunk
					chunkMap[viewedChunkCoord]->UpdateChunk();
				}
				//Create New Chunk
				else {
					const FTransform SpawnLocAndRotation;
					//Delay Actor Spawning to call appropriate functions first
					ABlockChunk* chunk = GetWorld()->SpawnActorDeferred<ABlockChunk>(ABlockChunk::StaticClass(), SpawnLocAndRotation);
					chunk->ConstructTerrainChunk(viewedChunkCoord, chunkSize, detailLevels, maxViewDist, viewerPosition, *mapGen, &visibleTerrainChunks);
					chunkMap.Add(viewedChunkCoord, chunk);
					//Spawn Actor
					chunk->FinishSpawning(SpawnLocAndRotation);
					chunk->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				}
			}
		}
	}
}