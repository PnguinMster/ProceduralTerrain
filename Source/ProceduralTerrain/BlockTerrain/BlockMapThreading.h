#pragma once

#include "CoreMinimal.h"
#include "BlockNoiseMap.h"
#include "HAL/Runnable.h"
#include "BlockMeshGenerator.h"
#include "BlockMeshSettings.h"
#include "BlockHeightMapSettings.h"
#include "BlockTextureGenerator.h"
#include "Curves/CurveFloat.h"
#include "BlockHeightMapGenerator.h"
#include "HAL/RunnableThread.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Components/ActorComponent.h"
#include "BlockMapThreading.generated.h"

//Create Delegate
DECLARE_DELEGATE_OneParam(FGeneratedMap, FHeightMap);
DECLARE_DELEGATE_OneParam(FGeneratedMesh, UMeshData*);

//Structs
template<typename T>
struct FThreadData
{
	TDelegate<void(T)>* callback;
	T parameter;

	FThreadData(TDelegate<void(T)>* callback, T parameter) {
		this->callback = callback;
		this->parameter = parameter;
	}
};

UCLASS()
class PROCEDURALTERRAIN_API ABlockMapThreading : public AActor
{
	GENERATED_BODY()
public:
	ABlockMapThreading();// Constructor

	UPROPERTY(EditDefaultsOnly)
	UBlockMeshSettings* meshSettings;
	UPROPERTY(EditDefaultsOnly)
	UBlockHeightMapSettings* heightMapSettings;

	//Threading
	TQueue<FThreadData<FHeightMap>*> mapDataThreadInfoQueue;
	TQueue<FThreadData<UMeshData*>*> meshDataThreadInfoQueue;

	void RequestMapData(FVector2D center, FGeneratedMap* callback);
	void RequestMeshData(FHeightMap heightMap, int lod, FGeneratedMesh* callback);

protected:
	// Default Function
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	//Delegate
	FGeneratedMap OnGeneratedMap;
	FGeneratedMesh OnGeneratedMesh;
};
/****************************
*
* Height Map Thread
*
*******************************/
class FHeightMapThread : public FRunnable
{
public:
	//Constructor
	FHeightMapThread(FVector2D center, FGeneratedMap* callback, UBlockHeightMapSettings* heightmapsettings,
		UBlockMeshSettings* meshSettings, TQueue<FThreadData<FHeightMap>*>* mapDataThreadInfoQueue);

	virtual ~FHeightMapThread() override;
private:
	//Variables
	FRunnableThread* Thread;
	FCriticalSection MapCritialSection;

	FGeneratedMap* generatedMap;
	FVector2D Center;
	UBlockHeightMapSettings* heightMapSettings;
	UBlockMeshSettings* meshSettings;
	TQueue<FThreadData<FHeightMap>*>* mapDataThreadInfoQueue;

	// Thread Overide Functions
	virtual bool Init() override { return true; }
	virtual uint32 Run() override;
};

/***************************
*
* Mesh Data Thread
*
*****************************/
class FMeshDataThread : public FRunnable
{
public:
	//Constructor
	FMeshDataThread(FHeightMap data, int lod, FGeneratedMesh* callback, int blocksize, TQueue<FThreadData<UMeshData*>*>* meshdatathreadinfoqueue);

	virtual ~FMeshDataThread() override;
private:
	//Variables
	FRunnableThread* Thread;
	FCriticalSection MeshCritialSection;

	FHeightMap heightData;
	int LOD;
	FGeneratedMesh* generatedMesh;
	int blockSize;
	TQueue<FThreadData<UMeshData*>*>* meshDataThreadInfoQueue;

	// Thread Overide Functions
	virtual bool Init() override { return true; }
	virtual uint32 Run() override;
};
