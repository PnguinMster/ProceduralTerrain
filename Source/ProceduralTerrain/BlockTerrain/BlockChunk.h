#pragma once

#include "CoreMinimal.h"
#include "BlockMapThreading.h"
#include "BlockHeightMapSettings.h"
#include "BlockChunk.generated.h"

DECLARE_DELEGATE(FConstructedLODMesh);

USTRUCT(BlueprintType)
struct FLODInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0", UIMin = "1.0"))
		int chunksVisible;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "6.0", UIMin = "0.0", UIMax = "6.0"))
		int lod;
};

UCLASS()
class PROCEDURALTERRAIN_API ABlockChunk : public AActor
{
	GENERATED_BODY()
	//Info Variables
	UProceduralMeshComponent* meshComponent;
	UMaterialInterface* dynamicMaterial;
	ABlockMapThreading* mapGen;
	TArray<ABlockChunk*>* chunkVisibleUpdate;
	FHeightMap heightMap;

	//Vector
	FVector OriginalPosition;
	FVector2D sampleCenter;
	FVector2D* viewPos;

	//Data
	int previousLODIndex = -1;
	float pivotOffset;
	float maxView;
	bool mapDataReceived;
	FVector2D Coord;

	//Array
	TArray<FLODInfo>* detailLevels;
	TArray<ULODMesh*> lodMeshes;

	//Delegates
	FConstructedLODMesh OnLodMeshConstruct;
	FGeneratedMap OnMapData;
	FGeneratedMesh OnMeshData;

	ABlockChunk(); //Constructor

	UFUNCTION()
		void OnMapDataRecieved(FHeightMap heightmap);

public:
	//Constructor Parameter Function
	void ConstructTerrainChunk(FVector2D coord, int size, TArray<FLODInfo>& detaillevels, float maxViewDist, FVector2D& viewerPosition, ABlockMapThreading& worldGenerator, TArray<ABlockChunk*>* chunkList);

	UFUNCTION()
		void UpdateChunk();

	FVector2D GetCoord() { return Coord; }

	//Visibility Functions
	void SetVisible(bool visible) { meshComponent->SetVisibility(visible); }
	const bool IsVisible() { return meshComponent->IsVisible(); }
};

UCLASS()
class ULODMesh : public UObject
{
	GENERATED_BODY()
public:
	//Info Variable
	FConstructedLODMesh* OnLodMeshConstruct;
	UProceduralMeshComponent* mesh;
	FGeneratedMesh OnMeshData;
	ABlockMapThreading* mapGen;
	UMeshData* MeshData;

	//Data
	bool hasRequestedMesh = false;
	bool hasMesh = false;
	int LOD = 0;

	//Constructor
	ULODMesh();

	//Constructor Parameter Function
	void ConstructLODMesh(int lod, ABlockMapThreading* mapgen, FConstructedLODMesh* onlodmeshconstruct);

	UFUNCTION()
		void OnMeshDataReceived(UMeshData* meshData);
	UFUNCTION()
		void RequestMesh(FHeightMap heightData);
};

