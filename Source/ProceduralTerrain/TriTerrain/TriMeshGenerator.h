#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "TriMeshGenerator.generated.h"

UCLASS()
class PROCEDURALTERRAIN_API UTriMeshData : public UObject
{
	GENERATED_BODY()

	//Mesh Info
	TArray<FVector>Vertices;
	TArray<int32>Triangles;
	TArray<FVector2D>UVs;
	TArray<FVector>Normals;

	TArray<FVector> borderVertices;
	TArray<int> borderTriangles;

	int triangleIndex;
	int borderTriangleIndex;
public:

	UTriMeshData() {}
	void Initialize(int verticesPerLine);
	void AddVertex(FVector vertexPosition, FVector2D uv, int vertexIndex);
	void AddTraingle(int a, int b, int c);
	void FlatShading();
	void CreateMesh(UProceduralMeshComponent* mesh);
};

class PROCEDURALTERRAIN_API TriMeshGenerator
{
public:
	static UTriMeshData* GenerateTerrainMesh(TArray<TArray<float>> heightMap, float heightMultiplier, UCurveFloat* heightCurve, int levelOfDetail);
};