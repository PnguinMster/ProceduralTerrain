#include "MeshData_Block.h"

UMeshData_Block::UMeshData_Block()
{
	Vertices.Reset();
	Triangles.Reset();
	Normals.Reset();
	Tangents.Reset();
	UVs.Reset();
	Colors.Reset();
}

void UMeshData_Block::SetArraysSize(int size)
{
	Vertices.SetNum(size);
	Normals.SetNum(size);
	UVs.SetNum(size);
}

void UMeshData_Block::CreateMesh(UProceduralMeshComponent* mesh)
{
	mesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, Colors, Tangents, true);
}

void UMeshData_Block::AddBlockVertices(int vertexIndex, float faceScale, FVector2D faceCoord, float heightClamp)
{
	Vertices[vertexIndex] = FVector(-faceScale + faceCoord.X, faceScale + faceCoord.Y, heightClamp);
	Vertices[vertexIndex + 1] = FVector(faceScale + faceCoord.X, faceScale + faceCoord.Y, heightClamp);
	Vertices[vertexIndex + 2] = FVector(-faceScale + faceCoord.X, -faceScale + faceCoord.Y, heightClamp);
	Vertices[vertexIndex + 3] = FVector(faceScale + faceCoord.X, -faceScale + faceCoord.Y, heightClamp);
}

void UMeshData_Block::AddTriangles(int index)
{
	Triangles.Add(index);
	Triangles.Add(index + 1);
	Triangles.Add(index + 2);

	Triangles.Add(index + 3);
	Triangles.Add(index + 2);
	Triangles.Add(index + 1);
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

void UMeshData_Block::AddExistingUVs(int uvIndex)
{
	FVector2D DefinedUV;
	UVs.Add(DefinedUV = UVs[uvIndex]);
	UVs.Add(DefinedUV = UVs[uvIndex + 1]);
	UVs.Add(DefinedUV = UVs[uvIndex + 2]);
	UVs.Add(DefinedUV = UVs[uvIndex + 3]);
}