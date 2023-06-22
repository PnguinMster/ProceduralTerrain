#pragma once

#include "ProceduralMeshComponent.h"
#include "CoreMinimal.h"
#include "MeshData_Block.generated.h"

UENUM()
enum class EFaceSide : uint8 { TOP, NORTH, SOUTH, WEST, EAST };

UCLASS()
class PROCEDURALTERRAIN_API UMeshData_Block : public UObject
{
	GENERATED_BODY()

protected:
	TArray<FVector>Vertices;
	TArray<int32>Triangles;
	TArray<FVector>Normals;
	TArray<FVector2D>UVs;

	int vertIndex;
	int triangleIndex;
public:
	UMeshData_Block();

	void SetArraysSize(int size);
	void CreateMesh(UProceduralMeshComponent* mesh);
	void AddVertices(EFaceSide faceSide, float faceScale, FVector2D faceCoord, float topHeight, float bottomHeight = 0);
	void AddTriangles();
	void AddNormals();
	void AddUVS(int X, int Y, float meshScale);
};
