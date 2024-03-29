#include "LODMesh_Tri.h"
#include "MeshGenerator_Tri.h"

ULODMesh_Tri::ULODMesh_Tri()
{
	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
}

void ULODMesh_Tri::Initialize(int lod, FVoidDelegate* updateCallback)
{
	MeshDataRecieved.BindUObject(this, &ULODMesh_Tri::OnMeshDataRecieved);
	Lod = lod;
	UpdateCallback = updateCallback;
}

void ULODMesh_Tri::OnMeshDataRecieved(UObject* meshDataObject)
{
	Cast<UMeshData_Tri>(meshDataObject)->CreateMesh(Mesh);
	HasMesh = true;

	UpdateCallback->Execute();
}

void ULODMesh_Tri::RequestMesh(UHeightMap_Tri* heightMap, UMeshSettings_Tri* meshSettings, UMapThreading* mapThread)
{
	TFunction<UObject* (void)> function = [=]() { return MeshGenerator_Tri::GenerateTerrainMesh(heightMap->Values, meshSettings, Lod); };
	HasRequestedMesh = true;

	mapThread->RequestData(function, &MeshDataRecieved);
}