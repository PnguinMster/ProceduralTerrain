#include "MeshGenerator_Tri.h"

UMeshData_Tri* MeshGenerator_Tri::GenerateTerrainMesh(TArray<TArray<float>>& heightMap, UMeshSettings_Tri* meshSettings, int levelOfDetail)
{
	int meshVertexIndex = 0;
	int verticesPerLine = meshSettings->numVertsPerLine;
	int outOfMeshVertexIndex = -1;
	int skipIncrement = (levelOfDetail == 0) ? 1 : levelOfDetail * 2;

	TArray<TArray<int>> vertexIndexMap;
	UMeshData_Tri* meshData = NewObject<UMeshData_Tri>();

	meshData->Initialize(verticesPerLine, skipIncrement);
	SetVertexIndexMap(vertexIndexMap, verticesPerLine, skipIncrement, outOfMeshVertexIndex, meshVertexIndex);
	CreateTopology(meshData, meshSettings, verticesPerLine, skipIncrement, vertexIndexMap, heightMap);
	meshData->FlatShading();

	return meshData;
}

void MeshGenerator_Tri::SetVertexIndexMap(TArray<TArray<int>>& vertexIndexMap, int verticesPerLine, int skipIncrement, int& outOfMeshVertexIndex, int& meshVertexIndex)
{
	vertexIndexMap.SetNum(verticesPerLine);
	for (int i = 0; i < verticesPerLine; i++)
		vertexIndexMap[i].SetNum(verticesPerLine);

	for (int y = 0; y < verticesPerLine; y++) {
		for (int x = 0; x < verticesPerLine; x++) {
			bool isOutOfMeshVertex = y == 0 || y == verticesPerLine - 1 || x == 0 || x == verticesPerLine - 1;
			bool isSkippedVertex = x > 2 && x < verticesPerLine - 3 && y > 2 && y < verticesPerLine - 3 && ((x - 2) % skipIncrement != 0 || (y - 2) % skipIncrement != 0);

			if (isOutOfMeshVertex) {
				vertexIndexMap[x][y] = outOfMeshVertexIndex;
				outOfMeshVertexIndex--;
			}
			else if (!isSkippedVertex) {
				vertexIndexMap[x][y] = meshVertexIndex;
				meshVertexIndex++;
			}
		}
	}
}

void MeshGenerator_Tri::CreateTopology(UMeshData_Tri* meshData, UMeshSettings_Tri* meshSettings, int verticesPerLine, int skipIncrement, TArray<TArray<int>>& vertexIndexMap, TArray<TArray<float>>& heightMap)
{
	FVector2D topLeft = FVector2D(-1, 1) * meshSettings->GetMeshWorldSize() / 2.f;

	for (int y = 0; y < verticesPerLine; y++) {
		for (int x = 0; x < verticesPerLine; x++) {
			bool isSkippedVertex = x > 2 && x < verticesPerLine - 3 && y > 2 && y < verticesPerLine - 3 && ((x - 2) % skipIncrement != 0 || (y - 2) % skipIncrement != 0);

			if (isSkippedVertex)
				continue;

			bool isOutOfMeshVertex = y == 0 || y == verticesPerLine - 1 || x == 0 || x == verticesPerLine - 1;
			bool isMeshEdgeVertex = (y == 1 || y == verticesPerLine - 2 || x == 1 || x == verticesPerLine - 2) && !isOutOfMeshVertex;
			bool isMainVertex = (x - 2) % skipIncrement == 0 && (y - 2) % skipIncrement == 0 && !isOutOfMeshVertex && !isMeshEdgeVertex;
			bool isEdgeConnectionVertex = (y == 2 || y == verticesPerLine - 3 || x == 2 || x == verticesPerLine - 3) && !isOutOfMeshVertex && !isMeshEdgeVertex && !isMainVertex;
			float height = GetHeight(isEdgeConnectionVertex, x, y, verticesPerLine, skipIncrement, heightMap);

			FVector2D percent = FVector2D(x - 1, y - 1) / (verticesPerLine - 3);
			FVector2D vertexPosition2D = topLeft + FVector2D(percent.X, -percent.Y) * meshSettings->GetMeshWorldSize();

			meshData->AddVertex(FVector(vertexPosition2D.X, vertexPosition2D.Y, height), percent, vertexIndexMap[x][y]);
			CreateTriangle(x, y, verticesPerLine, skipIncrement, isEdgeConnectionVertex, isMainVertex, meshData, vertexIndexMap);
		}
	}
}

float MeshGenerator_Tri::GetHeight(bool isEdgeConnectionVertex, int x, int y, int verticesPerLine, int skipIncrement, TArray<TArray<float>>& heightMap)
{
	if (!isEdgeConnectionVertex)
		return heightMap[x][y];

	bool isVertical = x == 2 || x == verticesPerLine - 3;
	int distToMainVertexA = ((isVertical) ? y - 2 : x - 2) % skipIncrement;
	int distToMainVertexB = skipIncrement - distToMainVertexA;

	float distPercentFromAToB = distToMainVertexA / (float)skipIncrement;
	float heightMainVertexA = heightMap[(isVertical) ? x : x - distToMainVertexA][(isVertical) ? y - distToMainVertexA : y];
	float heightMainVertexB = heightMap[(isVertical) ? x : x + distToMainVertexB][(isVertical) ? y + distToMainVertexB : y];

	return heightMainVertexA * (1 - distPercentFromAToB) + heightMainVertexB * distPercentFromAToB;
}

void MeshGenerator_Tri::CreateTriangle(int x, int y, int verticesPerLine, int skipIncrement, bool isEdgeConnectionVertex, bool isMainVertex, UMeshData_Tri* meshData, TArray<TArray<int>>& vertexIndexMap)
{
	bool createTriangle = x < verticesPerLine - 1 && y < verticesPerLine - 1 && (!isEdgeConnectionVertex || (x != 2 && y != 2));

	if (!createTriangle)
		return;

	int currentIncrement = (isMainVertex && x != verticesPerLine - 3 && y != verticesPerLine - 3) ? skipIncrement : 1;
	int a = vertexIndexMap[x][y];
	int b = vertexIndexMap[x + currentIncrement][y];
	int c = vertexIndexMap[x][y + currentIncrement];
	int d = vertexIndexMap[x + currentIncrement][y + currentIncrement];
	meshData->AddTraingle(a, d, c);
	meshData->AddTraingle(d, a, b);
}