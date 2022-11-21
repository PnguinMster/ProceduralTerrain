#include "TriTerrainGenerator.h"
#include "HeightMapGenerator_Tri.h"

ATriTerrainGenerator::ATriTerrainGenerator()
{
	PrimaryActorTick.bCanEverTick = true;

	mapThread = CreateDefaultSubobject<UTriMapThreading>(TEXT("MapThreader"));
}

// Called when the game starts or when spawned
void ATriTerrainGenerator::BeginPlay()
{
	Super::BeginPlay();

	chunksVisibleInViewDist = meshSettings->detailLevels[meshSettings->detailLevels.Num() - 1].visibleChunks;
	ViewerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	UpdateVisibleChunk();
}

// Called every frame
void ATriTerrainGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	viewerPosition.Set(ViewerPawn->GetActorLocation().X, ViewerPawn->GetActorLocation().Y);

	if ((viewerPositionOld - viewerPosition).SizeSquared() > sqrViewerMoveThresholdForChunkUpdate) {
		viewerPositionOld = viewerPosition;
		UpdateVisibleChunk();
	}
}

void ATriTerrainGenerator::UpdateVisibleChunk()
{
	TSet<FVector2D> alreadyUpdateChunkCoords;
	for (int i = visibleTerrainChunks.Num() - 1; i >= 0 ; i--) {
		alreadyUpdateChunkCoords.Add(visibleTerrainChunks[i]->coord);
		visibleTerrainChunks[i]->UpdateTerrainChunk();
	}

	int currentChunkCoordX = FMath::RoundToInt(viewerPosition.X / meshSettings->GetMeshWorldSize());
	int currentChunkCoordY = FMath::RoundToInt(viewerPosition.Y / meshSettings->GetMeshWorldSize());

	for (int yOffset = -chunksVisibleInViewDist; yOffset <= chunksVisibleInViewDist; yOffset++) {
		for (int xOffset = -chunksVisibleInViewDist; xOffset <= chunksVisibleInViewDist; xOffset++) {
			FVector2D viewedChunkCoord = FVector2D(currentChunkCoordX + xOffset, currentChunkCoordY + yOffset);

			if (!alreadyUpdateChunkCoords.Contains(viewedChunkCoord)) {
				if (terrainChunkMap.Contains(viewedChunkCoord)) {
					terrainChunkMap[viewedChunkCoord]->UpdateTerrainChunk();
				}
				else {
					const FTransform SpawnLocAndRotation;
					//Delay Actor Spawning to call appropriate functions first
					AChunk_Tri* chunk = GetWorld()->SpawnActorDeferred<AChunk_Tri>(AChunk_Tri::StaticClass(), SpawnLocAndRotation);
					chunk->Initialize(viewedChunkCoord, meshSettings, heightMapSettings, mapThread, viewerPosition, &visibleTerrainChunks);
					terrainChunkMap.Add(viewedChunkCoord, chunk);
					//Spawn Actor
					chunk->FinishSpawning(SpawnLocAndRotation);
					chunk->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				}
			}
		}
	}
}
