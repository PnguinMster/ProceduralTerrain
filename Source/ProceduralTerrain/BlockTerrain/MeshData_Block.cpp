#include "MeshData_Block.h"

UMeshData_Block::UMeshData_Block()
{
	Vertices.Reset();
	Triangles.Reset();
	Normals.Reset();
	UVs.Reset();
}

void UMeshData_Block::SetArraysSize(int size)
{
	Vertices.SetNum(size*4);
	Normals.SetNum(size*4);
	UVs.SetNum(size*4);
	Triangles.SetNum(size * 2 * 3);
}

void UMeshData_Block::CreateMesh(UProceduralMeshComponent* mesh)
{
	TArray<FLinearColor> EmptyColors;
	TArray<FProcMeshTangent> EmptyTangents;
	mesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, EmptyColors, EmptyTangents, true);
}

void UMeshData_Block::SetVertices(int vertexIndex, float faceScale, FVector2D faceCoord, float heightClamp)
{
	Vertices[vertexIndex] = FVector(-faceScale + faceCoord.X, faceScale + faceCoord.Y, heightClamp);
	Vertices[vertexIndex + 1] = FVector(faceScale + faceCoord.X, faceScale + faceCoord.Y, heightClamp);
	Vertices[vertexIndex + 2] = FVector(-faceScale + faceCoord.X, -faceScale + faceCoord.Y, heightClamp);
	Vertices[vertexIndex + 3] = FVector(faceScale + faceCoord.X, -faceScale + faceCoord.Y, heightClamp);
}

void UMeshData_Block::AddTriangles(int index)
{
	Triangles[triangleIndex] = index;
	Triangles[triangleIndex + 1] = index + 1;
	Triangles[triangleIndex + 2] = index + 2;

	Triangles[triangleIndex + 3] = index + 3;
	Triangles[triangleIndex + 4] = index + 2;
	Triangles[triangleIndex + 5] = index + 1;

	triangleIndex += 6;
}

void UMeshData_Block::SetNormals(int vertexIndex)
{
	FVector vertexNormal = FVector::CrossProduct(Vertices[vertexIndex + 2] - Vertices[vertexIndex + 1], Vertices[vertexIndex + 2] - Vertices[vertexIndex]).GetSafeNormal();
	for (int i = 0; i < 4; i++)
		Normals[vertexIndex + i] = vertexNormal;
}

void UMeshData_Block::AddUVS(int vertexIndex, int X, int Y, float meshScale)
{
	FVector2D uvScale = FVector2D((1.f / meshScale), (1.f / meshScale));
	FVector2D uvPos = FVector2D(X, Y) * uvScale;

	UVs[vertexIndex] = FVector2D(0.f, 1.f) * uvScale + uvPos;
	UVs[vertexIndex + 1] = FVector2D(1.f, 1.f) * uvScale + uvPos;
	UVs[vertexIndex + 2] = FVector2D(0.f, 0.f) * uvScale + uvPos;
	UVs[vertexIndex + 3] = FVector2D(1.f, 0.f) * uvScale + uvPos;
}