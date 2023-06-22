#include "MeshGenerator_Block.h"

UMeshData_Block* MeshGenerator_Block::GenerateTerrainMesh(TArray<TArray<float>> heightMap, int blockSize, int levelOfDetail)
{
	UMeshData_Block* meshData = NewObject<UMeshData_Block>();
	int meshSize = heightMap.Num() - 2;
	int meshSimplificationIncrement = (levelOfDetail == 0) ? 1 : levelOfDetail * 2;

	meshData->SetArraysSize(MeshFaceCount(heightMap, blockSize, meshSimplificationIncrement));

	for (int y = 0; y < meshSize; y += meshSimplificationIncrement) {
		for (int x = 0; x < meshSize; x += meshSimplificationIncrement) {
			CreateFace(meshData, EFaceSide::TOP, heightMap, blockSize, meshSimplificationIncrement, x, y);
			CreateFace(meshData, EFaceSide::NORTH, heightMap, blockSize, meshSimplificationIncrement, x, y);
			CreateFace(meshData, EFaceSide::EAST, heightMap, blockSize, meshSimplificationIncrement, x, y);
			CreateFace(meshData, EFaceSide::WEST, heightMap, blockSize, meshSimplificationIncrement, x, y);
			CreateFace(meshData, EFaceSide::SOUTH, heightMap, blockSize, meshSimplificationIncrement, x, y);
		}
	}
	return meshData;
}

int MeshGenerator_Block::MeshFaceCount(TArray<TArray<float>>& heightMap, int blockSize, int meshSimpleIncrement)
{
	int faceCount = 0;
	int meshSize = heightMap.Num() - 2;
	int verticesPerLine = (meshSize - 1) / meshSimpleIncrement + 1;

	for (int y = 0; y < meshSize; y += meshSimpleIncrement) {
		for (int x = 0; x < meshSize; x += meshSimpleIncrement) {
			int topHeight = GetBlockHeight(heightMap, blockSize, x + 1, y + 1);

			if (topHeight > GetBottomHeight(EFaceSide::EAST, heightMap, blockSize, meshSimpleIncrement, x, y)) faceCount++;
			if (topHeight > GetBottomHeight(EFaceSide::WEST, heightMap, blockSize, meshSimpleIncrement, x, y)) faceCount++;
			if (topHeight > GetBottomHeight(EFaceSide::NORTH, heightMap, blockSize, meshSimpleIncrement, x, y)) faceCount++;
			if (topHeight > GetBottomHeight(EFaceSide::SOUTH, heightMap, blockSize, meshSimpleIncrement, x, y)) faceCount++;
		}
	}

	faceCount += FMath::Square(verticesPerLine);

	return faceCount;
}

void MeshGenerator_Block::CreateFace(UMeshData_Block* meshData, EFaceSide side, TArray<TArray<float>>& heightMap, int blockSize, int meshSimpleIncrement, int x, int y)
{
	int halfExtent = blockSize * 0.5f;
	int meshSize = heightMap.Num() - 2;
	int verticesPerLine = (meshSize - 1) / meshSimpleIncrement + 1;
	int topHeight = GetBlockHeight(heightMap, blockSize, x + 1, y + 1);
	int bottomHeight = GetBottomHeight(side, heightMap, blockSize, meshSimpleIncrement, x, y);

	float topLeft = ((meshSize - 1) * -halfExtent) + (halfExtent * meshSimpleIncrement);
	float faceSize = halfExtent * meshSimpleIncrement;
	float spacing = blockSize;

	FVector2D facePosition = FVector2D(spacing * x + topLeft, spacing * y + topLeft);

	if (side == EFaceSide::TOP) {
		meshData->AddUVS(x, y, verticesPerLine);
		meshData->AddVertices(side, faceSize, facePosition, topHeight);
	}
	else if (topHeight > bottomHeight) {
		meshData->AddUVS(x, y, verticesPerLine);
		meshData->AddVertices(side, faceSize, facePosition, topHeight, bottomHeight);
	}
}

int MeshGenerator_Block::GetBottomHeight(EFaceSide side, TArray<TArray<float>>& heightMap, int blockSize, int meshSimpleIncrement, int x, int y)
{
	int heightOffset = (blockSize)*meshSimpleIncrement;

	switch (side)
	{
	case EFaceSide::EAST:
		return x + 1 + meshSimpleIncrement > heightMap.Num() - 1 ?
			GetBlockHeight(heightMap, blockSize, x + 1, y + 2) - heightOffset :
			GetBlockHeight(heightMap, blockSize, x + 1 + meshSimpleIncrement, y + 1);
		break;

	case EFaceSide::WEST:
		return x < meshSimpleIncrement ?
			GetBlockHeight(heightMap, blockSize, x, y + 1) - heightOffset :
			GetBlockHeight(heightMap, blockSize, x + 1 - meshSimpleIncrement, y + 1);
		break;

	case EFaceSide::NORTH:
		return y + 1 + meshSimpleIncrement > heightMap.Num() - 1 ?
			GetBlockHeight(heightMap, blockSize, x + 1, y + 2) - heightOffset :
			GetBlockHeight(heightMap, blockSize, x + 1, y + 1 + meshSimpleIncrement);
		break;

	case EFaceSide::SOUTH:
		return y < meshSimpleIncrement ?
			GetBlockHeight(heightMap, blockSize, x + 1, y) - heightOffset :
			GetBlockHeight(heightMap, blockSize, x + 1, y + 1 - meshSimpleIncrement);
		break;
	default:
		return 0;
		break;
	}
}

int MeshGenerator_Block::GetBlockHeight(TArray<TArray<float>>& heightMap, int blockSize, int x, int y)
{
	int blockClamp = heightMap[x][y] * blockSize;
	blockClamp -= blockClamp % blockSize;

	return blockClamp;
}