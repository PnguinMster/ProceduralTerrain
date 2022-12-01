#pragma once

#include "CoreMinimal.h"
#include "MapThreading_Block.h"
#include "ProceduralMeshComponent.h"
#include "HeightMapSettings_Block.h"
#include "MeshSettings_Block.h"
#include "LODMesh_Block.generated.h"

DECLARE_DELEGATE(FVoidDelegate);

UCLASS()
class PROCEDURALTERRAIN_API ULODMesh_Block : public UObject
{
	GENERATED_BODY()
public:
	FDataRecieved OnMeshData;
	FVoidDelegate* UpdateCallback;

	UProceduralMeshComponent* Mesh;

	bool HasRequestedMesh;
	bool HasMesh;
	int Lod;

	ULODMesh_Block();

	void Initialize(int lod, FVoidDelegate* onlodmeshconstruct);
	void OnMeshDataReceived(UObject* meshDataObject);
	void RequestMesh(UHeightMap_Block* heightMap, UMeshSettings_Block* meshSettings, UMapThreading_Block* mapThread);
};
