#include "LODMesh_Block.h"
#include "MeshGenerator_Block.h"

ULODMesh_Block::ULODMesh_Block()
{
	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	OnMeshData.BindUObject(this, &ULODMesh_Block::OnMeshDataReceived);
}

void ULODMesh_Block::Initialize(int lod, FVoidDelegate* updateCallback)
{
	Lod = lod;
	UpdateCallback = updateCallback;
}

void ULODMesh_Block::OnMeshDataReceived(UObject* meshDataObject)
{
	Cast<UMeshData_Block>(meshDataObject)->CreateMesh(Mesh);
	HasMesh = true;

	UpdateCallback->Execute();
}

void ULODMesh_Block::RequestMesh(UHeightMap_Block* heightMap, UMeshSettings_Block* meshSettings, UMapThreading_Block* mapThread)
{
	TFunction<UObject* (void)> function = [=]() { return MeshGenerator_Block::GenerateTerrainMesh(heightMap->Values, meshSettings->MeshScale, Lod); };
	HasRequestedMesh = true;

	mapThread->RequestData(function, &OnMeshData);
}