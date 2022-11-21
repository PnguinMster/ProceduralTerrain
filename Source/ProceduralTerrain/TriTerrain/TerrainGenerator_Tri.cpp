#include "TerrainGenerator_Tri.h"
#include "HeightMapGenerator_Tri.h"

ATerrainGenerator_Tri::ATerrainGenerator_Tri()
{
	PrimaryActorTick.bCanEverTick = true;

	mapThread = CreateDefaultSubobject<UTriMapThreading>(TEXT("MapThreader"));
}

// Called when the game starts or when spawned
void ATerrainGenerator_Tri::BeginPlay()
{
	Super::BeginPlay();

	chunksVisibleInView = meshSettings->detailLevels[meshSettings->detailLevels.Num() - 1].visibleChunks;
	ViewerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	UpdateVisibleChunk();
}

// Called every frame
void ATerrainGenerator_Tri::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	viewerPosition.Set(ViewerPawn->GetActorLocation().X, ViewerPawn->GetActorLocation().Y);

	if (FVector2D::Distance(viewerPosition, viewerPositionOld) > moveThresholdForChunkUpdate) {
		viewerPositionOld = viewerPosition;
		UpdateVisibleChunk();
	}
}

void ATerrainGenerator_Tri::UpdateVisibleChunk()
{
	TSet<FVector2D> updatedChunkCoords;
	for (int i = visibleChunks.Num() - 1; i >= 0 ; i--) {
		updatedChunkCoords.Add(visibleChunks[i]->coord);
		visibleChunks[i]->UpdateTerrainChunk();
	}

	int currentChunkCoordX = FMath::RoundToInt(viewerPosition.X / meshSettings->GetMeshWorldSize());
	int currentChunkCoordY = FMath::RoundToInt(viewerPosition.Y / meshSettings->GetMeshWorldSize());

	for (int yOffset = -chunksVisibleInView; yOffset <= chunksVisibleInView; yOffset++) {
		for (int xOffset = -chunksVisibleInView; xOffset <= chunksVisibleInView; xOffset++) {

			FVector2D viewedChunkCoord = FVector2D(currentChunkCoordX + xOffset, currentChunkCoordY + yOffset);

			if (updatedChunkCoords.Contains(viewedChunkCoord))
				continue;

			if (chunkMap.Contains(viewedChunkCoord))
				chunkMap[viewedChunkCoord]->UpdateTerrainChunk();

			if(!chunkMap.Contains(viewedChunkCoord)) 
				CreateChunk(viewedChunkCoord);
		}
	}
}

void ATerrainGenerator_Tri::CreateChunk(FVector2D& viewedChunkCoord)
{
	const FTransform SpawnLocAndRotation;

	AChunk_Tri* chunk = GetWorld()->SpawnActorDeferred<AChunk_Tri>(AChunk_Tri::StaticClass(), SpawnLocAndRotation);
	chunk->Initialize(mapThread, meshSettings, heightMapSettings, &visibleChunks, viewedChunkCoord, viewerPosition);

	chunkMap.Add(viewedChunkCoord, chunk);

	chunk->FinishSpawning(SpawnLocAndRotation);
	chunk->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}
