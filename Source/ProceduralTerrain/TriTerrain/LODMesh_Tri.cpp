#include "LODMesh_Tri.h"
#include "MeshGenerator_Tri.h"

ULODMesh_Tri::ULODMesh_Tri()
{
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	meshDataRecieved.BindUObject(this, &ULODMesh_Tri::OnMeshDataRecieved);
}

void ULODMesh_Tri::Initialize(int Lod, FVoidDelegate* UpdateCallback)
{
	lod = Lod;
	updateCallback = UpdateCallback;
}

void ULODMesh_Tri::OnMeshDataRecieved(UObject* meshDataObject)
{
	Cast<UMeshData_Tri>(meshDataObject)->CreateMesh(mesh);
	hasMesh = true;

	updateCallback->Execute();
}

void ULODMesh_Tri::RequestMesh(UHeightMap_Tri* heightMap, UTriMeshSettings* meshSettings, UMapThreading_Tri* mapThread)
{
	TFunction<UObject* (void)> function = [=]() { return MeshGenerator_Tri::GenerateTerrainMesh(heightMap->values, meshSettings, lod); };
	hasRequestedmesh = true;

	mapThread->RequestData(function, &meshDataRecieved);
}