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

	if (!dataQueue.IsEmpty()) {
		FThreadInfo threadInfo;
		dataQueue.Dequeue(threadInfo);
		threadInfo.callback->Execute(threadInfo.parameter);
	}
	/*
	if (!mapDataThreadInfoQueue.IsEmpty()) {
		FMapThreadInfo<UTri_HeightMap*> threadInfo;
		mapDataThreadInfoQueue.Dequeue(threadInfo);
		threadInfo.callback->Execute(threadInfo.parameter);
	}

	if (!meshDataThreadInfoQueue.IsEmpty()) {
		FMapThreadInfo<UTriMeshData*> threadInfo;
		meshDataThreadInfoQueue.Dequeue(threadInfo);
		threadInfo.callback->Execute(threadInfo.parameter);
	}
	*/
}

void ATriMapThreading::RequestData(TFunction<UObject* (void)> generateData, FDataRecieved* callback)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [generateData, callback, this]() {
		UObject* data = generateData();
		AsyncTask(ENamedThreads::GameThread, [callback, data, this]() {
			//mapDataThreadInfoQueue.Enqueue(FMapThreadInfo<UTri_HeightMap*>(callback, mapData));
			dataQueue.Enqueue(FThreadInfo(callback, data));
		});
	});
}

/*
void ATriMapThreading::RequestHeightData(FVector2D center, FMapReceived* callback)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [center, callback, this]() {
		UTri_HeightMap* mapData = HeightMapGenerator_Tri::GenerateHeightMap(meshSettings->numVertsPerLine, meshSettings->numVertsPerLine, heightMapSettings, center);

		AsyncTask(ENamedThreads::GameThread, [callback, mapData,this]() {
			mapDataThreadInfoQueue.Enqueue(FMapThreadInfo<UTri_HeightMap*>(callback, mapData));
		});
	});
}
void ATriMapThreading::RequestMeshData(UTri_HeightMap* heightMap, int lod, FMeshReceived* callback)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [heightMap, lod, callback, this]() {
		UTriMeshData* meshData = TriMeshGenerator::GenerateTerrainMesh(heightMap->values, meshSettings, lod);

		AsyncTask(ENamedThreads::GameThread, [callback, meshData, this]() {
			meshDataThreadInfoQueue.Enqueue(FMapThreadInfo<UTriMeshData*>(callback, meshData));
		});
	});
}
*/