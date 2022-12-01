#pragma once

#include "ProceduralMeshComponent.h"
#include "MeshSettings_Block.h"
#include "CoreMinimal.h"
#include "MeshData_Block.generated.h"

UCLASS()
class PROCEDURALTERRAIN_API UMeshData_Block : public UObject
{
	GENERATED_BODY()

public:
	TArray<FVector>Vertices;
	TArray<int32>Triangles;
	TArray<FVector>Normals;
	TArray<FProcMeshTangent>Tangents;
	TArray<FVector2D>UVs;
	TArray<FLinearColor>Colors;

	TArray<FVector>BorderVertices;

	UMeshData_Block();

	void SetArraysSize(int size);
	void CreateMesh(UProceduralMeshComponent* mesh);
	void AddBlockVertices(int vertexIndex, float faceScale, FVector2D faceCoord, float heightClamp);
	void AddTriangles(int index);
	void SetNormals(int vertexIndex);
	void AddUVS(int vertexIndex, int X, int Y, float meshScale);
	void AddExistingUVs(int uvIndex);
};
