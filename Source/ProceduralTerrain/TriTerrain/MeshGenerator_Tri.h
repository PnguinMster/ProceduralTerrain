#pragma once

#include "CoreMinimal.h"
#include "MeshData_Tri.h"
#include "TriMeshSettings.h"

class PROCEDURALTERRAIN_API MeshGenerator_Tri
{
public:
	static UMeshData_Tri* GenerateTerrainMesh(TArray<TArray<float>>& heightMap, UTriMeshSettings* meshSettings, int levelOfDetail);

protected:
	static void SetVertexIndexMap(TArray<TArray<int>>& vertexIndexMap, int verticesPerLine, int skipIncrement, int& outOfMeshVertexIndex, int& meshVertexIndex);
	static void CreateTopology(UMeshData_Tri* meshData, UTriMeshSettings* meshSettings, int verticesPerLine, int skipIncrement, TArray<TArray<int>>& vertexIndexMap, TArray<TArray<float>>& heightMap);
	static float GetHeight(bool isEdgeConnectionVertex, int x, int y, int verticesPerLine, int skipIncrement, TArray<TArray<float>>& heightMap);
	static void CreateTriangle(int x, int y, int verticesPerLine, int skipIncrement, bool isEdgeConnectionVertex, bool isMainVertex, UMeshData_Tri* meshData, TArray<TArray<int>>& vertexIndexMap);
};