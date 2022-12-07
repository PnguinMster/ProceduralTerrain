#include "MeshGenerator_Block.h"

UMeshData_Block* MeshGenerator_Block::GenerateTerrainMesh(TArray<TArray<float>> heightMap, int blockSize, int levelOfDetail)
{
	UMeshData_Block* meshData = NewObject<UMeshData_Block>();
	int meshSize = heightMap.Num() - 2;
	int meshSimplificationIncrement = (levelOfDetail == 0) ? 1 : levelOfDetail * 2;
	int verticesPerLine = (meshSize - 1) / meshSimplificationIncrement + 1;

	float topLeft = ((meshSize - 1) * -blockSize) + (blockSize * meshSimplificationIncrement);
	float faceSize = blockSize * meshSimplificationIncrement;
	float spacing = blockSize * 2.f;

	meshData->SetArraysSize(MeshFaceCount(heightMap, blockSize, verticesPerLine, meshSize,meshSimplificationIncrement));
	//meshData->SetArraysSize(verticesPerLine * verticesPerLine);

	for (int y = 0; y < meshSize; y += meshSimplificationIncrement) {
		for (int x = 0; x < meshSize; x += meshSimplificationIncrement) {
			FVector2D facePosition = FVector2D(spacing * x + topLeft, spacing * y + topLeft);
			int topHeight = GetBlockHeight(heightMap, blockSize, x + 1, y + 1);

			meshData->AddUVS(x, y, verticesPerLine);
			meshData->AddVertices(EFaceSide::TOP, faceSize, facePosition, topHeight);
			
			int bottomHeight = x + 1 + meshSimplificationIncrement > heightMap.Num() - 1 ? GetBlockHeight(heightMap, blockSize, x + 1, y + 2) - (blockSize * meshSimplificationIncrement) : GetBlockHeight(heightMap, blockSize, x + 1 + meshSimplificationIncrement, y + 1);
			if (topHeight > bottomHeight) {
				meshData->AddUVS(x, y, verticesPerLine);
				meshData->AddVertices(EFaceSide::EAST, faceSize, facePosition, topHeight, bottomHeight);
			}

			bottomHeight = x < meshSimplificationIncrement ? GetBlockHeight(heightMap, blockSize, x, y + 1) - (blockSize * meshSimplificationIncrement) : GetBlockHeight(heightMap, blockSize, x + 1 - meshSimplificationIncrement, y + 1);
			if (topHeight > bottomHeight) {
				meshData->AddUVS(x, y, verticesPerLine);
				meshData->AddVertices(EFaceSide::WEST, faceSize, facePosition, topHeight, bottomHeight);
			}

			bottomHeight = y + 1 + meshSimplificationIncrement > heightMap.Num() - 1 ? GetBlockHeight(heightMap, blockSize, x + 1, y + 2) - (blockSize * meshSimplificationIncrement) : GetBlockHeight(heightMap, blockSize, x + 1, y + 1 + meshSimplificationIncrement);
			if (topHeight > bottomHeight) {
				meshData->AddUVS(x, y, verticesPerLine);
				meshData->AddVertices(EFaceSide::NORTH, faceSize, facePosition, topHeight, bottomHeight);
			}

			bottomHeight = y < meshSimplificationIncrement ? GetBlockHeight(heightMap, blockSize, x + 1, y) - (blockSize * meshSimplificationIncrement) : bottomHeight = GetBlockHeight(heightMap, blockSize, x + 1, y + 1 - meshSimplificationIncrement);
			if (topHeight > bottomHeight) {
				meshData->AddUVS(x, y, verticesPerLine);
				meshData->AddVertices(EFaceSide::SOUTH, faceSize, facePosition, topHeight, bottomHeight);
			}
		}
	}
	return meshData;
}

int MeshGenerator_Block::GetBlockHeight(TArray<TArray<float>>& heightMap, int blockSize, int x, int y)
{
	int blockClamp = blockSize + (heightMap[x][y] * blockSize);
	blockClamp -= (blockClamp % (blockSize * 2));

	return blockClamp;
}

int MeshGenerator_Block::MeshFaceCount(TArray<TArray<float>>& heightMap, int blockSize, int verticesPerLine, int meshSize, int meshSimpleIncrement)
{
	int faceCount = 0;
	for (int y = 0; y < meshSize; y+= meshSimpleIncrement) {
		for (int x = 0; x < meshSize; x += meshSimpleIncrement) {
			int topHeight = GetBlockHeight(heightMap, blockSize, x + 1, y + 1);

			int bottomHeight = x + 1 + meshSimpleIncrement > heightMap.Num() - 1 ? GetBlockHeight(heightMap, blockSize, x + 1, y + 2) - (blockSize * meshSimpleIncrement) : GetBlockHeight(heightMap, blockSize, x + 1 + meshSimpleIncrement, y + 1);
			if (topHeight > bottomHeight) {
				faceCount++;
			}

			bottomHeight = x < meshSimpleIncrement ? GetBlockHeight(heightMap, blockSize, x, y + 1) - (blockSize * meshSimpleIncrement) : GetBlockHeight(heightMap, blockSize, x + 1 - meshSimpleIncrement, y + 1);
			if (topHeight > bottomHeight) {
				faceCount++;
			}

			bottomHeight = y + 1 + meshSimpleIncrement > heightMap.Num() - 1 ? GetBlockHeight(heightMap, blockSize, x + 1, y + 2) - (blockSize * meshSimpleIncrement) : GetBlockHeight(heightMap, blockSize, x + 1, y + 1 + meshSimpleIncrement);
			if (topHeight > bottomHeight) {
				faceCount++;
			}

			bottomHeight = y < meshSimpleIncrement ? GetBlockHeight(heightMap, blockSize, x + 1, y) - (blockSize * meshSimpleIncrement) : bottomHeight = GetBlockHeight(heightMap, blockSize, x + 1, y + 1 - meshSimpleIncrement);
			if (topHeight > bottomHeight) {
				faceCount++;
			}
		}
	}
	faceCount += FMath::Square(verticesPerLine);
	return faceCount;
}