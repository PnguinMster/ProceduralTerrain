#include "TriTerrainGenerator.h"
#include "TriTextureGenerator.h"

ATriTerrainGenerator::ATriTerrainGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATriTerrainGenerator::BeginPlay()
{
	Super::BeginPlay();

	chunkSize = (mapThread->mapChunkSize - 1);
	chunksVisibleInViewDist = detailLevels[detailLevels.Num() - 1].visibleChunks;
	maxViewDist = chunksVisibleInViewDist * chunkSize;
	ViewerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	UpdateVisibleChunk();
}

// Called every frame
void ATriTerrainGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	viewerPosition.Set(ViewerPawn->GetActorLocation().X / mapThread->meshSettings->unformScale, ViewerPawn->GetActorLocation().Y / mapThread->meshSettings->unformScale);

	if ((viewerPositionOld - viewerPosition).SizeSquared() > sqrViewerMoveThresholdForChunkUpdate) {
		viewerPositionOld = viewerPosition;
		UpdateVisibleChunk();
	}
}

void ATriTerrainGenerator::UpdateVisibleChunk()
{
	for (int i = 0; i < terrainChunksVisibleLastUpdate.Num(); i++) {
		terrainChunksVisibleLastUpdate[i]->SetVisible(false);
	}
	terrainChunksVisibleLastUpdate.Empty();

	int currentChunkCoordX = FMath::RoundToInt(viewerPosition.X / chunkSize);
	int currentChunkCoordY = FMath::RoundToInt(viewerPosition.Y / chunkSize);

	for (int yOffset = -chunksVisibleInViewDist; yOffset <= chunksVisibleInViewDist; yOffset++) {
		for (int xOffset = -chunksVisibleInViewDist; xOffset <= chunksVisibleInViewDist; xOffset++) {
			FVector2D viewedChunkCoord = FVector2D(currentChunkCoordX + xOffset, currentChunkCoordY + yOffset);

			if (terrainChunkMap.Contains(viewedChunkCoord)) {
				terrainChunkMap[viewedChunkCoord]->UpdateTerrainChunk();
			}
			else {
				const FTransform SpawnLocAndRotation;
				//Delay Actor Spawning to call appropriate functions first
				ATriChunk* chunk = GetWorld()->SpawnActorDeferred<ATriChunk>(ATriChunk::StaticClass(), SpawnLocAndRotation);
				chunk->Initialize(viewedChunkCoord, chunkSize, this);
				terrainChunkMap.Add(viewedChunkCoord, chunk);
				//Spawn Actor
				chunk->FinishSpawning(SpawnLocAndRotation);
				chunk->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			}
		}
	}
}

/**********
* Chunk
**********/

ATriChunk::ATriChunk()
{
	meshObject = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	RootComponent = meshObject;
}

void ATriChunk::Initialize(FVector2D coord, int size, ATriTerrainGenerator* actor)
{
	//DO NOT put in constructor
	//It Will Break
	MapRecievedDelegate.BindUObject(this, &ATriChunk::OnMapDataRecieved);
	UpdateChunkDelegate.BindUObject(this, &ATriChunk::UpdateTerrainChunk);

	terrainGen = actor;
	position = coord * size;
	FVector positionV3 = FVector(position.X, position.Y, 0) * actor->mapThread->meshSettings->unformScale;

	meshObject->SetWorldLocation(positionV3);
	meshObject->SetRelativeScale3D(FVector::OneVector * actor->mapThread->meshSettings->unformScale);
	SetVisible(false);

	lodMeshes.SetNum(actor->detailLevels.Num());
	for (int i = 0; i < lodMeshes.Num(); i++) {
		lodMeshes[i] = NewObject<UTriLODMesh>();
		lodMeshes[i]->Initialize(actor->detailLevels[i].lod, &UpdateChunkDelegate, actor->mapThread);
	}

	terrainGen->mapThread->RequestMapData(position, &MapRecievedDelegate);
}

UTexture2D* ATriChunk::ColorArrayToTexture(TArray<FColor> colors)
{
	UTexture2D* texture = UTexture2D::CreateTransient(colors.Num(), 1, PF_B8G8R8A8);

	if (!texture) return nullptr;
#if WITH_EDITORONLY_DATA
	texture->MipGenSettings = TMGS_NoMipmaps;
#endif
	texture->NeverStream = true;
	texture->SRGB = 0;
	texture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
	FTexture2DMipMap& Mip = texture->GetPlatformData()->Mips[0];
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(Data, colors.GetData(), colors.Num() * 4);
	Mip.BulkData.Unlock();
	texture->UpdateResource();
	return texture;
}

UTexture2D* ATriChunk::FloatArrayToTexture(TArray<float> heights)
{
	UTexture2D* texture = UTexture2D::CreateTransient(heights.Num(), 1, PF_R32_FLOAT);

	if (!texture) return nullptr;
#if WITH_EDITORONLY_DATA
	texture->MipGenSettings = TMGS_NoMipmaps;
#endif
	texture->NeverStream = true;
	texture->SRGB = 0;
	texture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
	FTexture2DMipMap& Mip = texture->GetPlatformData()->Mips[0];
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(Data, heights.GetData(), heights.Num() * 4);
	Mip.BulkData.Unlock();
	texture->UpdateResource();
	return texture;
}

void ATriChunk::SetTexture()
{
	//Add Texture to Material
	int regionsCount = terrainGen->mapThread->regions.Num();
	TArray<FColor> colors;
	TArray<float> heights;
	colors.SetNum(regionsCount);
	heights.SetNum(regionsCount);
	for (int i = 0; i < regionsCount; i++) {
		colors[i] = terrainGen->mapThread->regions[i].color;
		heights[i] = terrainGen->mapThread->regions[i].height;
	}

	UTexture2D* colorTexture = ColorArrayToTexture(colors);
	UTexture2D* heightTexture = FloatArrayToTexture(heights);

	UMaterialInstanceDynamic* dynamicMaterialInstance = UMaterialInstanceDynamic::Create(terrainGen->materialInterface, meshObject->GetOwner());
	dynamicMaterialInstance->SetScalarParameterValue("MinHeight", terrainGen->mapThread->meshSettings->GetMinHeight());
	dynamicMaterialInstance->SetScalarParameterValue("MaxHeight", terrainGen->mapThread->meshSettings->GetMaxHeight());
	dynamicMaterialInstance->SetScalarParameterValue("RegionCount", regionsCount);
	dynamicMaterialInstance->SetTextureParameterValue("ColorTexture", colorTexture);
	dynamicMaterialInstance->SetTextureParameterValue("HeightTexture", heightTexture);
	meshObject->SetMaterial(0, dynamicMaterialInstance);
}

void ATriChunk::OnMapDataRecieved(FMapData MapData)
{
	mapData = MapData;
	mapDataRecieved = true;

	SetTexture();
	UpdateTerrainChunk();
}

void ATriChunk::UpdateTerrainChunk()
{
	if (!mapDataRecieved) {
		return;
	}

	//float viewerDistFromNearestChunk = FVector::DistSquared(FVector(position.X, position.Y, 0), terrainGen->viewerPosition);
	float viewerDistFromNearestChunk = FVector2D::Distance(position, terrainGen->viewerPosition);
	bool visible = viewerDistFromNearestChunk <= terrainGen->maxViewDist;

	if (visible) {
		int lodIndex = 0;
		for (int i = 0; i < terrainGen->detailLevels.Num() - 1; i++) {
			if (viewerDistFromNearestChunk > terrainGen->detailLevels[i].visibleChunks * terrainGen->chunkSize)
				lodIndex = i + 1;
			else {
				break;
			}
		}
		if (lodIndex != previousLODIndex) {
			UTriLODMesh* lodMesh = lodMeshes[lodIndex];
			if (lodMesh->hasMesh) {
				previousLODIndex = lodIndex;
				meshObject->SetProcMeshSection(0, *lodMesh->mesh->GetProcMeshSection(0));
			}
			else if (!lodMesh->hasRequestedmesh) {
				lodMesh->RequestMesh(mapData);
			}
		}

		terrainGen->terrainChunksVisibleLastUpdate.Add(this);
	}
	SetVisible(visible);
}

/**********
* Lod Mesh
**********/
UTriLODMesh::UTriLODMesh() 
{
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	meshDataRecieved.BindUObject(this, &UTriLODMesh::OnMeshDataRecieved);
}

void UTriLODMesh::Initialize(int Lod, FVoidDelegate* UpdateCallback, ATriMapThreading* MapThread)
{
	updateCallback = UpdateCallback;
	lod = Lod;
	mapThread = MapThread;
}

void UTriLODMesh::OnMeshDataRecieved(UTriMeshData* meshData)
{
	if (!meshData->IsValidLowLevel()) {
		UE_LOG(LogTemp, Warning, TEXT("Mesh Data is null"))
		return;
	}

	meshData->CreateMesh(mesh);
	hasMesh = true;

	updateCallback->Execute();
}

void UTriLODMesh::RequestMesh(FMapData mapData)
{
	hasRequestedmesh = true;
	mapThread->RequestMeshData(mapData, lod, &meshDataRecieved);
}
