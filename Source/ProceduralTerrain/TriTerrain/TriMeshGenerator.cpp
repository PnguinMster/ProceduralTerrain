#include "TriMeshGenerator.h"

void UTriMeshData::Initialize(int verticesPerLine)
{
	Vertices.SetNum(verticesPerLine * verticesPerLine);
	UVs.SetNum(verticesPerLine * verticesPerLine);
	Triangles.SetNum((verticesPerLine - 1) * (verticesPerLine - 1) * 6);

	borderVertices.SetNum(verticesPerLine * 4 + 4);
	borderTriangles.SetNum(24 * verticesPerLine);
}

void UTriMeshData::AddVertex(FVector vertexPosition, FVector2D uv, int vertexIndex)
{
	if (vertexIndex < 0) {
		borderVertices[-vertexIndex - 1] = vertexPosition;
	}
	else {
		Vertices[vertexIndex] = vertexPosition;
		UVs[vertexIndex] = uv;
	}
}

void UTriMeshData::AddTraingle(int a, int b, int c)
{
	if (a < 0 || b < 0 || c < 0) {
		borderTriangles[borderTriangleIndex] = a;
		borderTriangles[borderTriangleIndex + 1] = b;
		borderTriangles[borderTriangleIndex + 2] = c;
		borderTriangleIndex += 3;
	}
	else {
		Triangles[triangleIndex] = a;
		Triangles[triangleIndex + 1] = b;
		Triangles[triangleIndex + 2] = c;
		triangleIndex += 3;
	}
}

void UTriMeshData::FlatShading()
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

void UTriMeshData::CreateMesh(UProceduralMeshComponent* mesh)
{	
	TArray<FLinearColor> EmptyColors;
	TArray<FProcMeshTangent> EmptyTangents;
	mesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, EmptyColors, EmptyTangents, true);
}


UTriMeshData* TriMeshGenerator::GenerateTerrainMesh(TArray<TArray<float>> heightMap, UTriMeshSettings* meshSettings, int levelOfDetail)
{
	int meshSimplificaiton = (levelOfDetail == 0) ? 1 : levelOfDetail * 2;
	int borderedSize = heightMap.Num();
	int meshSize = borderedSize - 2 * meshSimplificaiton;
	int meshSizeUnsimplified = borderedSize - 2;

	float topLeftX = (meshSizeUnsimplified - 1) / -2.f;
	float topLeftY = (meshSizeUnsimplified - 1) / 2.f;

	int verticesPerLine = (meshSize - 1) / meshSimplificaiton + 1;

	UTriMeshData* meshData = NewObject<UTriMeshData>();
	meshData->Initialize(verticesPerLine);
	
	TArray<TArray<int>> vertexIndexMap;
	vertexIndexMap.SetNum(borderedSize);
	for (int i = 0; i < borderedSize; i++)
		vertexIndexMap[i].SetNum(borderedSize);
	int meshVertexIndex = 0;
	int borderVertexIndex = -1;

	for (int y = 0; y < borderedSize; y += meshSimplificaiton) {
		for (int x = 0; x < borderedSize; x += meshSimplificaiton) {
			bool isBorderVertex = y == 0 || y == borderedSize - 1 || x == 0 || x == borderedSize - 1;

			if (isBorderVertex) {
				vertexIndexMap[x][y] = borderVertexIndex;
				borderVertexIndex--;
			}
			else {
				vertexIndexMap[x][y] = meshVertexIndex;
				meshVertexIndex++;
			}
		}
	}

	for (int y = 0; y < borderedSize; y+=meshSimplificaiton) {
		for (int x = 0; x < borderedSize; x+=meshSimplificaiton) {
			int vertexIndex = vertexIndexMap[x][y];
			FVector2D percent = FVector2D((x - meshSimplificaiton) / (float)meshSize, (y - meshSimplificaiton) / (float)meshSize);
			float height = heightMap[x][y];
			FVector vertexPosition = FVector(topLeftX + percent.X * meshSizeUnsimplified, topLeftY - percent.Y * meshSizeUnsimplified, height) * meshSettings->meshScale;

			meshData->AddVertex(vertexPosition, percent, vertexIndex);

			if (x < borderedSize -1 && y < borderedSize -1) {
				int a = vertexIndexMap[x][y];
				int b = vertexIndexMap[x + meshSimplificaiton][y];
				int c = vertexIndexMap[x][y + meshSimplificaiton];
				int d = vertexIndexMap[x + meshSimplificaiton][y + meshSimplificaiton];
				meshData->AddTraingle(a, d, c);
				meshData->AddTraingle(d, a, b);
			}
			vertexIndex++;
		}
	}
	meshData->FlatShading();

	return meshData;
}
