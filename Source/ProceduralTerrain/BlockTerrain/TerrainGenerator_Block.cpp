#include "TerrainGenerator_Block.h"

ATerrainGenerator_Block::ATerrainGenerator_Block()
{
	PrimaryActorTick.bCanEverTick = true;

	MapThread = CreateDefaultSubobject<UMapThreading_Block>(TEXT("MapThreader"));
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
	TSet<FVector2D> updatedChunkCoord;
	for (int i = VisibleChunks.Num() - 1; i >= 0; i--) {
		updatedChunkCoord.Add(VisibleChunks[i]->Coord);
		VisibleChunks[i]->UpdateChunk();
	}

	int currentChunkCoordX = FMath::RoundToInt(ViewerPosition.X / MeshSettings->GetMeshWorldScale());
	int currentChunkCoordY = FMath::RoundToInt(ViewerPosition.Y / MeshSettings->GetMeshWorldScale());

	//Update Chunk Visibility
	for (int yOffset = -ChunksVisibleInView; yOffset <= ChunksVisibleInView; yOffset++) {
		for (int xOffset = -ChunksVisibleInView; xOffset <= ChunksVisibleInView; xOffset++) {
			FVector2D viewedChunkCoord = FVector2D(currentChunkCoordX + xOffset, currentChunkCoordY + yOffset);

			if (updatedChunkCoord.Contains(viewedChunkCoord))
				continue;

			if (ChunkMap.Contains(viewedChunkCoord))
				ChunkMap[viewedChunkCoord]->UpdateChunk();

			if (!ChunkMap.Contains(viewedChunkCoord)) {
				CreateChunk(viewedChunkCoord);
			}
		}
	}
}

void ATerrainGenerator_Block::CreateChunk(FVector2D& viewedChunkCoord)
{
	const FTransform SpawnLocAndRotation;

	AChunk_Block* chunk = GetWorld()->SpawnActorDeferred<AChunk_Block>(AChunk_Block::StaticClass(), SpawnLocAndRotation);
	chunk->Initialize(MapThread, MeshSettings, HeightMapSettings, &VisibleChunks, viewedChunkCoord, ViewerPosition);

	ChunkMap.Add(viewedChunkCoord, chunk);

	chunk->FinishSpawning(SpawnLocAndRotation);
	chunk->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}