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
	Vertices.SetNum(size * 4);
	Normals.SetNum(size * 4);
	UVs.SetNum(size * 4);
	Triangles.SetNum(size * 2 * 3);
}

void UMeshData_Block::CreateMesh(UProceduralMeshComponent* mesh)
{
	TArray<FLinearColor> EmptyColors;
	TArray<FProcMeshTangent> EmptyTangents;
	mesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, EmptyColors, EmptyTangents, true);
}

void UMeshData_Block::AddVertices(EFaceSide faceSide, float faceScale, FVector2D faceCoord, float topHeight, float bottomHeight)
{
	switch (faceSide)
	{
	case EFaceSide::TOP:
		Vertices[vertIndex] = FVector(-faceScale + faceCoord.X, faceScale + faceCoord.Y, topHeight);
		Vertices[vertIndex + 1] = FVector(faceScale + faceCoord.X, faceScale + faceCoord.Y, topHeight);
		Vertices[vertIndex + 2] = FVector(-faceScale + faceCoord.X, -faceScale + faceCoord.Y, topHeight);
		Vertices[vertIndex + 3] = FVector(faceScale + faceCoord.X, -faceScale + faceCoord.Y, topHeight);
		break;

	case EFaceSide::NORTH:
		Vertices[vertIndex] = FVector(-faceScale + faceCoord.X, faceScale + faceCoord.Y, topHeight);
		Vertices[vertIndex + 1] = FVector(-faceScale + faceCoord.X, faceScale + faceCoord.Y, bottomHeight);
		Vertices[vertIndex + 2] = FVector(faceScale + faceCoord.X, faceScale + faceCoord.Y, topHeight);
		Vertices[vertIndex + 3] = FVector(faceScale + faceCoord.X, faceScale + faceCoord.Y, bottomHeight);
		break;

	case EFaceSide::SOUTH:
		Vertices[vertIndex] = FVector(faceScale + faceCoord.X, -faceScale + faceCoord.Y, topHeight);
		Vertices[vertIndex + 1] = FVector(faceScale + faceCoord.X, -faceScale + faceCoord.Y, bottomHeight);
		Vertices[vertIndex + 2] = FVector(-faceScale + faceCoord.X, -faceScale + faceCoord.Y, topHeight);
		Vertices[vertIndex + 3] = FVector(-faceScale + faceCoord.X, -faceScale + faceCoord.Y, bottomHeight);
		break;

	case EFaceSide::WEST:
		Vertices[vertIndex] = FVector(-faceScale + faceCoord.X, -faceScale + faceCoord.Y, topHeight);
		Vertices[vertIndex + 1] = FVector(-faceScale + faceCoord.X, -faceScale + faceCoord.Y, bottomHeight);
		Vertices[vertIndex + 2] = FVector(-faceScale + faceCoord.X, faceScale + faceCoord.Y, topHeight);
		Vertices[vertIndex + 3] = FVector(-faceScale + faceCoord.X, faceScale + faceCoord.Y, bottomHeight);
		break;

	case EFaceSide::EAST:
		Vertices[vertIndex] = FVector(faceScale + faceCoord.X, faceScale + faceCoord.Y, topHeight);
		Vertices[vertIndex + 1] = FVector(faceScale + faceCoord.X, faceScale + faceCoord.Y, bottomHeight);
		Vertices[vertIndex + 2] = FVector(faceScale + faceCoord.X, -faceScale + faceCoord.Y, topHeight);
		Vertices[vertIndex + 3] = FVector(faceScale + faceCoord.X, -faceScale + faceCoord.Y, bottomHeight);
		break;
	}

	AddNormals();
	AddTriangles();
	vertIndex += 4;
}

void UMeshData_Block::AddTriangles()
{
	Triangles[triangleIndex] = vertIndex;
	Triangles[triangleIndex + 1] = vertIndex + 1;
	Triangles[triangleIndex + 2] = vertIndex + 2;

	Triangles[triangleIndex + 3] = vertIndex + 3;
	Triangles[triangleIndex + 4] = vertIndex + 2;
	Triangles[triangleIndex + 5] = vertIndex + 1;

	triangleIndex += 6;
}

void UMeshData_Block::AddNormals()
{
	FVector vertexNormal = FVector::CrossProduct(Vertices[vertIndex + 2] - Vertices[vertIndex + 1], Vertices[vertIndex + 2] - Vertices[vertIndex]).GetSafeNormal();
	for (int i = 0; i < 4; i++)
		Normals[vertIndex + i] = vertexNormal;
}

void UMeshData_Block::AddUVS(int X, int Y, float meshScale)
{
	FVector2D uvScale = FVector2D((1.f / meshScale), (1.f / meshScale));
	FVector2D uvPos = FVector2D(X, Y) * uvScale;

	UVs[vertIndex] = FVector2D(0.f, 1.f) * uvScale + uvPos;
	UVs[vertIndex + 1] = FVector2D(1.f, 1.f) * uvScale + uvPos;
	UVs[vertIndex + 2] = FVector2D(0.f, 0.f) * uvScale + uvPos;
	UVs[vertIndex + 3] = FVector2D(1.f, 0.f) * uvScale + uvPos;
}