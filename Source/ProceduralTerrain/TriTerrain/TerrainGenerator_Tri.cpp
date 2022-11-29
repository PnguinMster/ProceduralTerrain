#include "TerrainGenerator_Tri.h"
#include "HeightMapGenerator_Tri.h"

ATerrainGenerator_Tri::ATerrainGenerator_Tri()
{
	PrimaryActorTick.bCanEverTick = true;

	MapThread = CreateDefaultSubobject<UMapThreading_Tri>(TEXT("MapThreader"));
}

void ATerrainGenerator_Tri::BeginPlay()
{
	Super::BeginPlay();

	ChunksVisibleInView = MeshSettings->DetailLevels[MeshSettings->DetailLevels.Num() - 1].VisibleChunks;
	ViewerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	UpdateVisibleChunk();
}

void ATerrainGenerator_Tri::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	ViewerPosition.Set(ViewerPawn->GetActorLocation().X, ViewerPawn->GetActorLocation().Y);

	if (FVector2D::Distance(ViewerPosition, ViewerPositionOld) > MoveThresholdForChunkUpdate) {
		ViewerPositionOld = ViewerPosition;
		UpdateVisibleChunk();
	}
}

void ATerrainGenerator_Tri::UpdateVisibleChunk()
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

void ATerrainGenerator_Tri::CreateChunk(FVector2D& viewedChunkCoord)
{
	const FTransform SpawnLocAndRotation;

	AChunk_Tri* chunk = GetWorld()->SpawnActorDeferred<AChunk_Tri>(AChunk_Tri::StaticClass(), SpawnLocAndRotation);
	chunk->Initialize(MapThread, MeshSettings, HeightMapSettings, &VisibleChunks, viewedChunkCoord, ViewerPosition);

	ChunkMap.Add(viewedChunkCoord, chunk);

	chunk->FinishSpawning(SpawnLocAndRotation);
	chunk->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}