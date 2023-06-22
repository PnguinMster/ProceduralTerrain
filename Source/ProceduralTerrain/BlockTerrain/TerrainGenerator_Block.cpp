#include "TerrainGenerator_Block.h"
#include "HeightMapGenerator_Block.h"

ATerrainGenerator_Block::ATerrainGenerator_Block()
{
	PrimaryActorTick.bCanEverTick = true;

	MapThread = CreateDefaultSubobject<UMapThreading>(TEXT("MapThreader"));
}

void ATerrainGenerator_Block::BeginPlay()
{
	Super::BeginPlay();

	ChunksVisibleInView = MeshSettings->DetailLevels[MeshSettings->DetailLevels.Num() - 1].ChunksVisible;
	ViewerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	UpdateVisibleChunks();
}

void ATerrainGenerator_Block::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	ViewerPosition.Set(ViewerPawn->GetActorLocation().X, ViewerPawn->GetActorLocation().Y);

	if (FVector2D::Distance(ViewerPosition, ViewerPositionOld) > MoveThresholdForChunkUpdate) {
		ViewerPositionOld = ViewerPosition;
		UpdateVisibleChunks();
	}
}

void ATerrainGenerator_Block::UpdateVisibleChunks()
{
	TSet<FVector2D> updatedChunkCoords;
	for (int i = VisibleChunks.Num() - 1; i >= 0; i--) {
		updatedChunkCoords.Add(VisibleChunks[i]->Coord);
		VisibleChunks[i]->UpdateChunk();
	}

	int currentChunkCoordX = FMath::RoundToInt(ViewerPosition.X / MeshSettings->GetMeshWorldSize());
	int currentChunkCoordY = FMath::RoundToInt(ViewerPosition.Y / MeshSettings->GetMeshWorldSize());

	for (int yOffset = -ChunksVisibleInView; yOffset <= ChunksVisibleInView; yOffset++) {
		for (int xOffset = -ChunksVisibleInView; xOffset <= ChunksVisibleInView; xOffset++) {
			FVector2D viewedChunkCoord = FVector2D(currentChunkCoordX + xOffset, currentChunkCoordY + yOffset);

			if (updatedChunkCoords.Contains(viewedChunkCoord))
				continue;

			if (ChunkMap.Contains(viewedChunkCoord))
				ChunkMap[viewedChunkCoord]->UpdateChunk();

			if (!ChunkMap.Contains(viewedChunkCoord))
				CreateChunk(viewedChunkCoord);
		}
	}
}

void ATerrainGenerator_Block::CreateChunk(FVector2D& viewedChunkCoord)
{
	const FTransform SpawnLocAndRotation;

	AChunk_Block* chunk = GetWorld()->SpawnActorDeferred<AChunk_Block>(AChunk_Block::StaticClass(), SpawnLocAndRotation);
	chunk->Initialize(MapThread, MeshSettings, HeightMapSettings, &VisibleChunks, viewedChunkCoord, ViewerPosition);

	ChunkMap.Add(viewedChunkCoord, chunk);
	chunk->FinishSpawning(SpawnLocAndRotation, false);
	chunk->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}