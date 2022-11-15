#include "TriMapThreading.h"
#include "TriTextureGenerator.h"
#include "TriMeshGenerator.h"
#include "HeightMapGenerator_Tri.h"
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
	//static ConstructorHelpers::FObjectFinder<UMaterial>asset(TEXT("/Game/Blueprints/BlockTerrain/Assets/MAT_BlockTerrain"));
	//dynamicMaterial = asset.Object;
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
		FMapThreadInfo<FTri_HeightMap> threadInfo;
		mapDataThreadInfoQueue.Dequeue(threadInfo);
		threadInfo.callback->Execute(threadInfo.parameter);
	}

	if (!meshDataThreadInfoQueue.IsEmpty()) {
		FMapThreadInfo<UTriMeshData*> threadInfo;
		meshDataThreadInfoQueue.Dequeue(threadInfo);
		threadInfo.callback->Execute(threadInfo.parameter);
	}
}

void ATriMapThreading::DrawMapInEditor()
{
	FTri_HeightMap mapData = HeightMapGenerator_Tri::GenerateHeightMap(meshSettings->numVertsPerLine, meshSettings->numVertsPerLine, heightMapSettings, FVector2D::ZeroVector);

	UTriMeshData* meshData = TriMeshGenerator::GenerateTerrainMesh(mapData.values, meshSettings, 0);
	meshData->CreateMesh(terrainMesh);
	/*
	//Add Texture to Material
	UMaterialInstanceDynamic* dynamicMaterialInstance = UMaterialInstanceDynamic::Create(dynamicMaterial, terrainMesh->GetOwner());
	dynamicMaterialInstance->SetTextureParameterValue("ChunkTexture", texture2D);
	terrainMesh->SetMaterial(0, dynamicMaterialInstance);
	*/
}

void ATriMapThreading::RequestHeightData(FVector2D center, FMapReceived* callback)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [center, callback, this]() {
		FTri_HeightMap mapData = HeightMapGenerator_Tri::GenerateHeightMap(meshSettings->numVertsPerLine, meshSettings->numVertsPerLine, heightMapSettings, center);

		AsyncTask(ENamedThreads::GameThread, [callback, mapData,this]() {
			mapDataThreadInfoQueue.Enqueue(FMapThreadInfo<FTri_HeightMap>(callback, mapData));
		});
	});
}

void ATriMapThreading::RequestMeshData(FTri_HeightMap heightMap, int lod, FMeshReceived* callback)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [heightMap, lod, callback, this]() {
		UTriMeshData* meshData = TriMeshGenerator::GenerateTerrainMesh(heightMap.values, meshSettings, lod);

		AsyncTask(ENamedThreads::GameThread, [callback, meshData, this]() {
			meshDataThreadInfoQueue.Enqueue(FMapThreadInfo<UTriMeshData*>(callback, meshData));
		});
	});
}