#include "BlockMapThreading.h"

// Sets default values
ABlockMapThreading::ABlockMapThreading()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABlockMapThreading::BeginPlay()
{
	Super::BeginPlay();
}

void ABlockMapThreading::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Map Data Thread Queue
	if (!mapDataThreadInfoQueue.IsEmpty()) {
		while (!mapDataThreadInfoQueue.IsEmpty())
		{
			FThreadData<FHeightMap>* threadInfo;
			mapDataThreadInfoQueue.Dequeue(threadInfo);
			threadInfo->callback->Execute(threadInfo->parameter);
			delete threadInfo;
		}
	}
	//Mesh Data Thread Queue
	if (!meshDataThreadInfoQueue.IsEmpty()) {
		while (!meshDataThreadInfoQueue.IsEmpty())
		{
			FThreadData<UMeshData*>* threadInfo;
			meshDataThreadInfoQueue.Dequeue(threadInfo);
			threadInfo->callback->Execute(threadInfo->parameter);
			delete threadInfo;
		}
	}
}

void ABlockMapThreading::RequestMapData(FVector2D center, FGeneratedMap* callback)
{
	//Create Map Data Thread
	FHeightMapThread* mapGenThread = new FHeightMapThread(center, callback, heightMapSettings, meshSettings, &mapDataThreadInfoQueue);
	delete mapGenThread;
}

void ABlockMapThreading::RequestMeshData(FHeightMap heightMap, int lod, FGeneratedMesh* callback)
{
	//Create Mesh Data Thread
	FMeshDataThread* meshGenThread = new FMeshDataThread(heightMap, lod, callback, meshSettings->BlockSize, &meshDataThreadInfoQueue);
	delete meshGenThread;
}

/****************************
*
* Height Map Thread
*
*******************************/
FHeightMapThread::FHeightMapThread(FVector2D center, FGeneratedMap* callback, UBlockHeightMapSettings* heightmapsettings, UBlockMeshSettings* meshsettings, TQueue<FThreadData<FHeightMap>*>* mapdatathreadinfoqueue)
{
	Center = center;
	generatedMap = callback;
	heightMapSettings = heightmapsettings;
	meshSettings = meshsettings;
	mapDataThreadInfoQueue = mapdatathreadinfoqueue;
	//Create Thread
	Thread = FRunnableThread::Create(this, TEXT("Map Generator Thread"));
}

uint32 FHeightMapThread::Run()
{
	//Get Map Data
	int size = meshSettings->chunkSize + 2;
	FHeightMap heightMap = BlockHeightMapGenerator::GenerateHeightMap(size, size, heightMapSettings, Center);
	heightMap.color = BlockTextureGenerator::GenerateColorMap(heightMap.values, &meshSettings->regions, heightMapSettings->heightMultiplier);

	//Lock Variable for Thread Safety
	FScopeLock lock(&MapCritialSection);
	mapDataThreadInfoQueue->Enqueue(new FThreadData<FHeightMap>(generatedMap, heightMap));
	lock.Unlock();
	return 0;
}

FHeightMapThread::~FHeightMapThread()
{
	if (Thread) {
		Thread->Kill();
		delete Thread;
	}
}

/***************************
*
* Mesh Data Thread
*
*****************************/
FMeshDataThread::FMeshDataThread(FHeightMap data, int lod, FGeneratedMesh* callback, int blocksize, TQueue<FThreadData<UMeshData*>*>* meshdatathreadinfoqueue)
{
	generatedMesh = callback;
	heightData = data;
	LOD = lod;
	blockSize = blocksize;
	meshDataThreadInfoQueue = meshdatathreadinfoqueue;
	//Create Thread
	Thread = FRunnableThread::Create(this, TEXT("Mesh Generator Thread"));
}

uint32 FMeshDataThread::Run()
{
	//Generate Procedural Mesh Data
	UMeshData* meshData = nullptr;
	meshData = BlockMeshGenerator::GenerateTerrainMesh(heightData.values, blockSize, LOD);

	//Lock Variable for Thread Safety
	FScopeLock lock(&MeshCritialSection);
	meshDataThreadInfoQueue->Enqueue(new FThreadData<UMeshData*>(generatedMesh, meshData));
	lock.Unlock();
	return 0;
}

FMeshDataThread::~FMeshDataThread()
{
	if (Thread) {
		Thread->Kill();
		delete Thread;
	}
}
