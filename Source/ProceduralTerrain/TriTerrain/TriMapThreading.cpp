#include "TriMapThreading.h"
#include "TriTextureGenerator.h"
#include "TriMeshGenerator.h"
#include "Async/Async.h"
#include "TriNoiseMap.h"

// Sets default values
ATriMapThreading::ATriMapThreading()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Mesh Component
	terrainMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	RootComponent = terrainMesh;

	//Assign Material
	// Load the base material from your created material. 
	static ConstructorHelpers::FObjectFinder<UMaterial>asset(TEXT("/Game/Blueprints/BlockTerrain/Assets/MAT_BlockTerrain"));
	dynamicMaterial = asset.Object;
}

// Called when the game starts or when spawned
void ATriMapThreading::BeginPlay()
{
	Super::BeginPlay();
	
	//DrawMapInEditor();
}

// Called every frame
void ATriMapThreading::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!mapDataThreadInfoQueue.IsEmpty()) {
		FMapThreadInfo<FMapData> threadInfo;
		mapDataThreadInfoQueue.Dequeue(threadInfo);
		threadInfo.callback->Execute(threadInfo.parameter);
	}

	if (!meshDataThreadInfoQueue.IsEmpty()) {
		FMapThreadInfo<UTriMeshData*> threadInfo;
		meshDataThreadInfoQueue.Dequeue(threadInfo);
		threadInfo.callback->Execute(threadInfo.parameter);
	}
}

FMapData ATriMapThreading::GenerateMapData(FVector2D center)
{
	TArray<TArray<float>> noiseMap = TriNoiseMap::GenerateNoiseMap(mapChunkSize + 2, mapChunkSize + 2, heightMapSettings->seed, heightMapSettings->noiseScale, heightMapSettings-> octaves, heightMapSettings->persistance, heightMapSettings->lacunarity, center + heightMapSettings->offset);
	/*
	TArray<uint8> colorMap;
	colorMap.SetNum(mapChunkSize * mapChunkSize * 4);
	for (int y = 0; y < mapChunkSize; y++) {
		for (int x = 0; x < mapChunkSize; x++) {
			float currentHeight = noiseMap[x][y];
			for (int i = 0; i < regions.Num(); i++) {
				if (currentHeight >= regions[i].height) {
					int currentPixelIndex = y * mapChunkSize + x;
					colorMap[4 * currentPixelIndex] = regions[i].color.B;
					colorMap[4 * currentPixelIndex + 1] = regions[i].color.G;
					colorMap[4 * currentPixelIndex + 2] = regions[i].color.R;
					colorMap[4 * currentPixelIndex + 3] = regions[i].color.A;
				}
				else
					break;
			}
		}
	}
	*/
	return FMapData(noiseMap);
}

void ATriMapThreading::DrawMapInEditor()
{
	FMapData mapData = GenerateMapData(FVector2D::ZeroVector);
	/*
	UTexture2D* texture2D;
	if (drawMode == ETriDrawMode::NoiseMap)
		texture2D = TriTextureGenerator::TextureFromHeightMap(mapData.noiseMap);
	else
		texture2D = TriTextureGenerator::TextureFromColorMap(mapData.color, mapChunkSize, mapChunkSize);
	*/

	UTriMeshData* meshData = TriMeshGenerator::GenerateTerrainMesh(mapData.noiseMap, meshSettings->meshHeightMultiplier, meshSettings->meshHeightCurve, editorPreviewLOD);
	meshData->CreateMesh(terrainMesh);
	/*
	//Add Texture to Material
	UMaterialInstanceDynamic* dynamicMaterialInstance = UMaterialInstanceDynamic::Create(dynamicMaterial, terrainMesh->GetOwner());
	dynamicMaterialInstance->SetTextureParameterValue("ChunkTexture", texture2D);
	terrainMesh->SetMaterial(0, dynamicMaterialInstance);
	*/
}

void ATriMapThreading::RequestMapData(FVector2D center, FMapReceived* callback)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [center, callback, this]() {
		FMapData mapData = GenerateMapData(center);

		AsyncTask(ENamedThreads::GameThread, [callback, mapData,this]() {
			mapDataThreadInfoQueue.Enqueue(FMapThreadInfo<FMapData>(callback, mapData));
		});
	});
}

void ATriMapThreading::RequestMeshData(FMapData mapData, int lod, FMeshReceived* callback)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [mapData, lod, callback, this]() {
		UTriMeshData* meshData = TriMeshGenerator::GenerateTerrainMesh(mapData.noiseMap, meshSettings->meshHeightMultiplier, meshSettings->meshHeightCurve, lod);

		AsyncTask(ENamedThreads::GameThread, [callback, meshData, this]() {
			meshDataThreadInfoQueue.Enqueue(FMapThreadInfo<UTriMeshData*>(callback, meshData));
		});
	});
}