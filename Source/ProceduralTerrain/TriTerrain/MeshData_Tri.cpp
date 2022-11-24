#include "MeshData_Tri.h"

void UMeshData_Tri::Initialize(int numVertsPerLine, int skipIncrement)
{
	int numMeshEdgeVertices = (numVertsPerLine - 2) * 4 - 4;
	int numEdgeConnectionVertices = (skipIncrement - 1) * (numVertsPerLine - 5) / skipIncrement * 4;
	int numMainVerticesPerLine = (numVertsPerLine - 5) / skipIncrement + 1;
	int numMainVertices = numMainVerticesPerLine * numMainVerticesPerLine;

	Vertices.SetNum(numMeshEdgeVertices + numEdgeConnectionVertices + numMainVertices);
	UVs.SetNum(Vertices.Num());

	int numMeshEdgeTriangles = 8 * (numVertsPerLine - 4);
	int numMainTriangles = (numMainVerticesPerLine - 1) * (numMainVerticesPerLine - 1) * 2;
	Triangles.SetNum((numMeshEdgeTriangles + numMainTriangles) * 3);

	outOfMeshVertices.SetNum(numVertsPerLine * 4 - 4);
	outOfMeshTraingles.SetNum(24 * (numVertsPerLine - 2));
}

void UMeshData_Tri::AddVertex(FVector vertexPosition, FVector2D uv, int vertexIndex)
{
	if (vertexIndex < 0)
		outOfMeshVertices[-vertexIndex - 1] = vertexPosition;
	else {
		Vertices[vertexIndex] = vertexPosition;
		UVs[vertexIndex] = uv;
	}
}

void UMeshData_Tri::AddTraingle(int a, int b, int c)
{
	if (a < 0 || b < 0 || c < 0) {
		outOfMeshTraingles[borderTriangleIndex] = a;
		outOfMeshTraingles[borderTriangleIndex + 1] = b;
		outOfMeshTraingles[borderTriangleIndex + 2] = c;
		borderTriangleIndex += 3;
	}
	else {
		Triangles[outOfMeshTriangleIndex] = a;
		Triangles[outOfMeshTriangleIndex + 1] = b;
		Triangles[outOfMeshTriangleIndex + 2] = c;
		outOfMeshTriangleIndex += 3;
	}
}

void UMeshData_Tri::FlatShading()
{
	AddFlatShadedVertices();

	Normals.SetNum(Vertices.Num());
	int triangleCount = Triangles.Num() / 3;
	for (int i = 0; i < triangleCount; i++) {
		int normalTriangleIndex = i * 3;
		FVector a = Vertices[normalTriangleIndex];
		FVector b = Vertices[normalTriangleIndex + 1];
		FVector c = Vertices[normalTriangleIndex + 2];
		FVector normalizedVector = FVector::CrossProduct(c - b, c - a).GetSafeNormal();
		Normals[normalTriangleIndex] = normalizedVector;
		Normals[normalTriangleIndex + 1] = normalizedVector;
		Normals[normalTriangleIndex + 2] = normalizedVector;
	}
}

void UMeshData_Tri::AddFlatShadedVertices()
{
	TArray<FVector> flatShadedVertices;
	TArray<FVector2D> flatShadedUVs;
	flatShadedVertices.SetNum(Triangles.Num());
	flatShadedUVs.SetNum(Triangles.Num());

	for (int i = 0; i < Triangles.Num(); i++) {
		flatShadedVertices[i] = Vertices[Triangles[i]];
		flatShadedUVs[i] = UVs[Triangles[i]];
		Triangles[i] = i;
	}

	Vertices = flatShadedVertices;
	UVs = flatShadedUVs;
}

void UMeshData_Tri::CreateMesh(UProceduralMeshComponent* mesh)
{
	TArray<FLinearColor> EmptyColors;
	TArray<FProcMeshTangent> EmptyTangents;
	mesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, EmptyColors, EmptyTangents, true);
}