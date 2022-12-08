#pragma once

#include "CoreMinimal.h"
#include "MeshData_Block.h"

class PROCEDURALTERRAIN_API MeshGenerator_Block
{
public:
	static UMeshData_Block* GenerateTerrainMesh(TArray<TArray<float>> heightMap, int blockSize, int levelOfDetail);

private:
	static int MeshFaceCount(TArray<TArray<float>>& heightMap, int blockSize, int meshSimpleIncrement);
	static void CreateFace(UMeshData_Block* meshData, EFaceSide side, TArray<TArray<float>>& heightMap, int blockSize, int meshSimpleIncrement, int x, int y);
	static int GetBottomHeight(EFaceSide side, TArray<TArray<float>>& heightMap, int blockSize, int meshSimpleIncrement, int x, int y);
	static int GetBlockHeight(TArray<TArray<float>>& heightMap, int blockSize, int x, int y);
};
