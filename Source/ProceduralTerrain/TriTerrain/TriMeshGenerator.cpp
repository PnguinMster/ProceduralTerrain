#include "TriMeshGenerator.h"

void UTriMeshData::Initialize(int numVertsPerLine, int skipIncrement)
{
	int numMeshEdgeVertices = (numVertsPerLine - 2) * 4 - 4;
	int numEdgeConnectionVertices = (skipIncrement - 1) * (numVertsPerLine - 5) / skipIncrement * 4;
	int numMainVerticesPerLine = (numVertsPerLine - 5) / skipIncrement + 1;
	int numMainVertices = numMainVerticesPerLine * numMainVerticesPerLine;

	Vertices.SetNum(numMeshEdgeVertices + numEdgeConnectionVertices + numMainVertices);
	UVs.SetNum(Vertices.Num());

	int numMeshEdgeTriangles = 8 * (numVertsPerLine-4);
	int numMainTriangles = (numMainVerticesPerLine - 1) * (numMainVerticesPerLine - 1) * 2;
	Triangles.SetNum((numMeshEdgeTriangles + numMainTriangles) * 3);

	outOfMeshVertices.SetNum(numVertsPerLine * 4 - 4);
	outOfMeshTraingles.SetNum(24 * (numVertsPerLine-2));
}

void UTriMeshData::AddVertex(FVector vertexPosition, FVector2D uv, int vertexIndex)
{
	if (vertexIndex < 0) {
		outOfMeshVertices[-vertexIndex - 1] = vertexPosition;
	}
	else {
		Vertices[vertexIndex] = vertexPosition;
		UVs[vertexIndex] = uv;
	}
}

void UTriMeshData::AddTraingle(int a, int b, int c)
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
	int skipIncrement = (levelOfDetail == 0) ? 1 : levelOfDetail * 2;
	int numVertsPerLine = meshSettings->numVertsPerLine;

	FVector2D topLeft = FVector2D(-1, 1) * meshSettings->GetMeshWorldSize() / 2.f;

	UTriMeshData* meshData = NewObject<UTriMeshData>();
	meshData->Initialize(numVertsPerLine, skipIncrement);
	
	TArray<TArray<int>> vertexIndexMap;
	vertexIndexMap.SetNum(numVertsPerLine);
	for (int i = 0; i < numVertsPerLine; i++)
		vertexIndexMap[i].SetNum(numVertsPerLine);
	int meshVertexIndex = 0;
	int outOfMeshVertexIndex = -1;

	for (int y = 0; y < numVertsPerLine; y++) {
		for (int x = 0; x < numVertsPerLine; x++) {
			bool isOutOfMeshVertex = y == 0 || y == numVertsPerLine - 1 || x == 0 || x == numVertsPerLine - 1;
			bool isSkippedVertex = x > 2 && x < numVertsPerLine - 3 && y > 2 && y < numVertsPerLine - 3 && ((x - 2) % skipIncrement != 0 || (y - 2) % skipIncrement != 0);
			if (isOutOfMeshVertex) {
				vertexIndexMap[x][y] = outOfMeshVertexIndex;
				outOfMeshVertexIndex--;
			}
			else if(!isSkippedVertex){
				vertexIndexMap[x][y] = meshVertexIndex;
				meshVertexIndex++;
			}
		}
	}

	for (int y = 0; y < numVertsPerLine; y++) {
		for (int x = 0; x < numVertsPerLine; x++) {
			bool isSkippedVertex = x > 2 && x < numVertsPerLine - 3 && y > 2 && y < numVertsPerLine - 3 && ((x - 2) % skipIncrement != 0 || (y - 2) % skipIncrement != 0);
			
			if(!isSkippedVertex) {
				bool isOutOfMeshVertex = y == 0 || y == numVertsPerLine - 1 || x == 0 || x == numVertsPerLine - 1;
				bool isMeshEdgeVertex = (y == 1 || y == numVertsPerLine - 2 || x == 1 || x == numVertsPerLine - 2) && !isOutOfMeshVertex;
				bool isMainVertex = (x - 2) % skipIncrement == 0 && (y - 2) % skipIncrement == 0 && !isOutOfMeshVertex && !isMeshEdgeVertex;
				bool isEdgeConnectionVertex = (y == 2 || y == numVertsPerLine - 3 || x == 2 || x == numVertsPerLine - 3) && !isOutOfMeshVertex && !isMeshEdgeVertex && !isMainVertex;

				int vertexIndex = vertexIndexMap[x][y];
				FVector2D percent = FVector2D(x - 1, y - 1) / (numVertsPerLine - 3);
				FVector2D vertexPosition2D = topLeft + FVector2D(percent.X,-percent.Y) * meshSettings->GetMeshWorldSize();
				float height = heightMap[x][y];
				
				if (isEdgeConnectionVertex) {
					bool isVertical = x == 2 || x == numVertsPerLine - 3;
					int distToMainVertexA = ((isVertical) ? y - 2 : x - 2) % skipIncrement;
					int distToMainVertexB = skipIncrement - distToMainVertexA;
					float distPercentFromAToB = distToMainVertexA / (float)skipIncrement;

					float heightMainVertexA = heightMap[(isVertical) ? x : x - distToMainVertexA][(isVertical) ? y - distToMainVertexA : y];
					float heightMainVertexB = heightMap[(isVertical) ? x : x + distToMainVertexB][(isVertical) ? y + distToMainVertexB : y];
					
					height = heightMainVertexA * (1 - distPercentFromAToB) + heightMainVertexB * distPercentFromAToB;
				}

				meshData->AddVertex(FVector(vertexPosition2D.X, vertexPosition2D.Y, height), percent, vertexIndex);

				bool createTriangle = x < numVertsPerLine - 1 && y < numVertsPerLine - 1 && (!isEdgeConnectionVertex || (x != 2 && y != 2));

				if (createTriangle) {
					int currentIncrement = (isMainVertex && x != numVertsPerLine - 3 && y != numVertsPerLine - 3) ? skipIncrement : 1;

					int a = vertexIndexMap[x][y];
					int b = vertexIndexMap[x + currentIncrement][y];
					int c = vertexIndexMap[x][y + currentIncrement];
					int d = vertexIndexMap[x + currentIncrement][y + currentIncrement];
					meshData->AddTraingle(a, d, c);
					meshData->AddTraingle(d, a, b);
				}
			}
		}
	}
	meshData->FlatShading();

	return meshData;
}
