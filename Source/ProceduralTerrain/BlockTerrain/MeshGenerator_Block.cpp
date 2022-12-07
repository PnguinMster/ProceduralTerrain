#include "MeshGenerator_Block.h"

UMeshData_Block* MeshGenerator_Block::GenerateTerrainMesh(TArray<TArray<float>> heightMap, int blockSize, int levelOfDetail)
{
	UMeshData_Block* meshData = NewObject<UMeshData_Block>();
	int meshSize = heightMap.Num() - 2;
	int meshSimplificationIncrement = (levelOfDetail == 0) ? 1 : levelOfDetail * 2;
	int verticesPerLine = (meshSize - 1) / meshSimplificationIncrement + 1;

	float topLeftX = ((meshSize - 1) * -blockSize) + (blockSize * meshSimplificationIncrement);
	float topLeftY = ((meshSize - 1) * -blockSize) + (blockSize * meshSimplificationIncrement);
	float faceSize = blockSize * meshSimplificationIncrement;
	float spacing = blockSize * 2.f;

	int blockNum = 0;
	for (int y = 0; y < meshSize; y++) {
		for (int x = 0; x < meshSize; x++) {
			int blockClamp = GetBlockHeight(heightMap, blockSize, x + 1, y + 1);

			if (blockClamp > GetBlockHeight(heightMap, blockSize, x + 2, y + 1))
				blockNum++;

			if (blockClamp > GetBlockHeight(heightMap, blockSize, x, y + 1))
				blockNum++;

			if (blockClamp > GetBlockHeight(heightMap, blockSize, x + 1, y + 2))
				blockNum++;

			if (blockClamp > GetBlockHeight(heightMap, blockSize, x + 1, y))
				blockNum++;
		}
	}
	blockNum += FMath::Square(verticesPerLine);

	int TriSize = FMath::Square(verticesPerLine) * 2 * 4;
	meshData->SetArraysSize(blockNum);

	for (int y = 0; y < meshSize; y += meshSimplificationIncrement) {
		for (int x = 0; x < meshSize; x += meshSimplificationIncrement) {
			FVector2D facePosition = FVector2D(spacing * x + topLeftX, spacing * y + topLeftY);
			int blockClamp = GetBlockHeight(heightMap, blockSize, x + 1, y + 1);

			meshData->AddUVS(x, y, verticesPerLine);
			meshData->AddVertices(EFaceSide::TOP, faceSize, facePosition, blockClamp);

			if (blockClamp > GetBlockHeight(heightMap, blockSize, x + 2, y + 1)) {
				meshData->AddUVS(x, y, verticesPerLine);
				meshData->AddVertices(EFaceSide::EAST, faceSize, facePosition, blockClamp, GetBlockHeight(heightMap, blockSize, x + 2, y + 1));
			}

			if (blockClamp > GetBlockHeight(heightMap, blockSize, x, y + 1)) {
				meshData->AddUVS(x, y, verticesPerLine);
				meshData->AddVertices(EFaceSide::WEST, faceSize, facePosition, blockClamp, GetBlockHeight(heightMap, blockSize, x, y + 1));
			}

			if (blockClamp > GetBlockHeight(heightMap, blockSize, x + 1, y + 2)) {
				meshData->AddUVS(x, y, verticesPerLine);
				meshData->AddVertices(EFaceSide::NORTH, faceSize, facePosition, blockClamp, GetBlockHeight(heightMap, blockSize, x + 1, y + 2));
			}

			if (blockClamp > GetBlockHeight(heightMap, blockSize, x + 1, y)) {
				meshData->AddUVS(x, y, verticesPerLine);
				meshData->AddVertices(EFaceSide::SOUTH, faceSize, facePosition, blockClamp, GetBlockHeight(heightMap, blockSize, x + 1, y));
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