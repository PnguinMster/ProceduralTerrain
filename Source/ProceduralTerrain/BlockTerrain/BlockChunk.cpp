#include "BlockChunk.h"

//Terrain Chunk
ABlockChunk::ABlockChunk() {
	//Create Mesh Component
	meshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("MeshComponent"));
	RootComponent = meshComponent;

	//Assign Material
	// Load the base material from your created material. 
	static ConstructorHelpers::FObjectFinder<UMaterial>asset(TEXT("/Game/Blueprints/BlockTerrain/Assets/MAT_BlockTerrain"));
	dynamicMaterial = asset.Object;
}

void ABlockChunk::OnMapDataRecieved(FHeightMap heightmap)
{
	heightMap = heightmap;
	mapDataReceived = true;
	UpdateChunk();
}

void ABlockChunk::ConstructTerrainChunk(FVector2D coord, int size, TArray<FLODInfo>& detaillevels, float maxViewDist, FVector2D& viewerPosition, ABlockMapThreading& worldGenerator, TArray<ABlockChunk*>* chunkList)
{
	//Assign Function to Delegate
	OnMapData.BindUObject(this, &ABlockChunk::OnMapDataRecieved);
	OnLodMeshConstruct.BindUObject(this, &ABlockChunk::UpdateChunk);

	//Set Values
	chunkVisibleUpdate = chunkList;
	detailLevels = &detaillevels;
	mapGen = &worldGenerator;
	maxView = maxViewDist;
	viewPos = &viewerPosition;
	sampleCenter = coord * size;
	Coord = coord;

	//Get Position as Vector 3
	FVector positionV3 = FVector(sampleCenter.X, sampleCenter.Y, 0);
	SetVisible(false);

	//Create LOD Meshes
	lodMeshes.SetNum(detaillevels.Num());
	for (int i = 0; i < (*detailLevels).Num(); i++) {
		lodMeshes[i] = NewObject<ULODMesh>();
		lodMeshes[i]->ConstructLODMesh((*detailLevels)[i].lod, &worldGenerator, &OnLodMeshConstruct);
	}

	//Set Position and Visibility
	RootComponent->SetWorldLocation(positionV3);
	pivotOffset = size * 0.5f;
	OriginalPosition = positionV3;
	worldGenerator.RequestMapData(coord * (mapGen->meshSettings->chunkSize), &OnMapData);
}

void ABlockChunk::UpdateChunk()
{
	if (mapDataReceived)
	{
		//Calculate Chunk's Visualized
		float viewerDistFromNearestEdge = FVector2D::Distance(FVector2D(sampleCenter.X, sampleCenter.Y), *viewPos);
		bool wasVisible = IsVisible();
		bool visible = viewerDistFromNearestEdge <= maxView;

		if (visible) {
			int lodIndex = 0;
			float chunkSize = mapGen->meshSettings->BlockSize * mapGen->meshSettings->chunkSize * 2.f;
			//Checks which Level of detail chunk should be
			for (int i = 0; i < (*detailLevels).Num() - 1; i++) {
				if (viewerDistFromNearestEdge > (*detailLevels)[i].chunksVisible * chunkSize)
					lodIndex = i + 1;
				else
					break;
			}
			//Sets new LOD
			if (lodIndex != previousLODIndex) {
				ULODMesh* lodMesh = lodMeshes[lodIndex];
				if (lodMesh->hasMesh) {
					//Create Appropriate LOD mesh
					previousLODIndex = lodIndex;
					lodMesh->MeshData->CreateMesh(meshComponent);

					UTexture2D* texture2D;
					if (mapGen->meshSettings->drawMode == EDrawMode::NoiseMap) 
						texture2D = BlockTextureGenerator::TextureFromHeightMap(heightMap.values, mapGen->heightMapSettings->heightMultiplier);
					else 
						texture2D = BlockTextureGenerator::TextureFromColorMap(heightMap.color, mapGen->meshSettings->chunkSize, mapGen->meshSettings->chunkSize);
					
					BlockTextureGenerator::DrawTexture(texture2D, dynamicMaterial, meshComponent);
				}
				//Request Mesh Info for LOD Mesh
				else if (!lodMesh->hasRequestedMesh)
					lodMesh->RequestMesh(heightMap);
			}
		}

		//Check if self is visible
		if (wasVisible != visible) {
			if (visible)
				chunkVisibleUpdate->Add(this);
			else
				chunkVisibleUpdate->Remove(this);
		}

		SetVisible(visible);
	}
}

ULODMesh::ULODMesh()
{
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("MeshComponent"));
	OnMeshData.BindUObject(this, &ULODMesh::OnMeshDataReceived);
}

void ULODMesh::ConstructLODMesh(int lod, ABlockMapThreading* mapgen, FConstructedLODMesh* onlodmeshconstruct)
{
	LOD = lod;
	mapGen = mapgen;
	OnLodMeshConstruct = onlodmeshconstruct;
}

void ULODMesh::OnMeshDataReceived(UMeshData* meshData)
{
	MeshData = meshData;
	hasMesh = true;

	OnLodMeshConstruct->Execute();
}

void ULODMesh::RequestMesh(FHeightMap heightData)
{
	hasRequestedMesh = true;
	mapGen->RequestMeshData(heightData, LOD, &OnMeshData);
}
