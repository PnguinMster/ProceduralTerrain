#pragma once

#include "CoreMinimal.h"
#include "MapThreading_Tri.h"
#include "ProceduralMeshComponent.h"
#include "HeightMapSettings_Tri.h"
#include "MeshSettings_Tri.h"
#include "LODMesh_Tri.generated.h"

DECLARE_DELEGATE(FVoidDelegate);

UCLASS()
class PROCEDURALTERRAIN_API ULODMesh_Tri : public UObject
{
	GENERATED_BODY()

public:
	FDataRecieved MeshDataRecieved;
	FVoidDelegate* UpdateCallback;

	UProceduralMeshComponent* Mesh;

	bool HasMesh;
	bool HasRequestedmesh;
	int Lod;

	ULODMesh_Tri();
	void Initialize(int lod, FVoidDelegate* updateCallback);
	void OnMeshDataRecieved(UObject* meshDataObject);
	void RequestMesh(UHeightMap_Tri* heightMap, UMeshSettings_Tri* meshSettings, UMapThreading_Tri* mapThread);
};
