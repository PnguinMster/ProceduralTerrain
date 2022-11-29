#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "MeshData_Tri.generated.h"

UCLASS()
class PROCEDURALTERRAIN_API UMeshData_Tri : public UObject
{
	GENERATED_BODY()

protected:
	TArray<FVector>Vertices;
	TArray<int32>Triangles;
	TArray<FVector2D>UVs;
	TArray<FVector>Normals;

	TArray<FVector> OutOfMeshVertices;
	TArray<int> OutOfMeshTraingles;

	int OutOfMeshTriangleIndex;
	int BorderTriangleIndex;

public:

	UMeshData_Tri() {}
	void Initialize(int verticesPerLine, int skipCount);
	void AddVertex(FVector vertexPosition, FVector2D uv, int vertexIndex);
	void AddTraingle(int a, int b, int c);
	void FlatShading();
	void CreateMesh(UProceduralMeshComponent* mesh);

protected:
	void AddFlatShadedVertices();
};
