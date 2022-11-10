#pragma once

#include "ProceduralMeshComponent.h"
#include "BlockMeshSettings.h"
#include "CoreMinimal.h"
#include "BlockMeshGenerator.generated.h"

UCLASS()
class UMeshData : public UObject
{
	GENERATED_BODY()

public:
	//Mesh Info
	TArray<FVector>Vertices;
	TArray<int32>Triangles;
	TArray<FVector>Normals;
	TArray<FProcMeshTangent>Tangents;
	TArray<FVector2D>UVs;
	TArray<FLinearColor>Colors;

	TArray<FVector>BorderVertices;

	//Constructor
	UMeshData();

	void SetArraysSize(int size);
	void CreateMesh(UProceduralMeshComponent* mesh);
	void AddBlockVertices(int vertexIndex, float faceScale, FVector2D faceCoord, float heightClamp);
	void AddTriangles(int index);
	void SetNormals(int vertexIndex);
	void AddUVS(int vertexIndex, int X, int Y, float meshScale);
	void AddExistingUVs(int uvIndex);
};

class PROCEDURALTERRAIN_API BlockMeshGenerator
{
public:
	static UMeshData* GenerateTerrainMesh(TArray<TArray<float>> heightMap, int blockSize, int levelOfDetail);
private:
	static int GetBlockHeight(TArray<TArray<float>>& heightMap, int blockSize, int x, int y);
};
