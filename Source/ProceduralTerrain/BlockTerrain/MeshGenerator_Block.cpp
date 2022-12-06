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
	int vertIndex = 0;

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

			meshData->SetVertices(vertIndex, faceSize, facePosition, blockClamp);
			meshData->SetNormals(vertIndex);
			meshData->AddUVS(vertIndex, x, y, verticesPerLine);
			meshData->AddTriangles(vertIndex);

			vertIndex += 4;

			if (blockClamp > GetBlockHeight(heightMap, blockSize, x + 2, y + 1)) {
				meshData->Vertices[vertIndex] = FVector(faceSize + facePosition.X, faceSize + facePosition.Y, blockClamp);
				meshData->Vertices[vertIndex + 1] = FVector(faceSize + facePosition.X, faceSize + facePosition.Y, GetBlockHeight(heightMap, blockSize, x + 2, y + 1));
				meshData->Vertices[vertIndex + 2] = FVector(faceSize + facePosition.X, -faceSize + facePosition.Y, blockClamp);
				meshData->Vertices[vertIndex + 3] = FVector(faceSize + facePosition.X, -faceSize + facePosition.Y, GetBlockHeight(heightMap, blockSize, x + 2, y + 1));

				meshData->AddTriangles(vertIndex);

				meshData->SetNormals(vertIndex);

				FVector2D uvScale = FVector2D((1.f / verticesPerLine), (1.f / verticesPerLine));
				FVector2D uvPos = FVector2D(x, y) * uvScale;

				meshData->UVs[vertIndex] = FVector2D(0.f, 1.f) * uvScale + uvPos;
				meshData->UVs[vertIndex + 1] = FVector2D(1.f, 1.f) * uvScale + uvPos;
				meshData->UVs[vertIndex + 2] = FVector2D(0.f, 0.f) * uvScale + uvPos;
				meshData->UVs[vertIndex + 3] = FVector2D(1.f, 0.f) * uvScale + uvPos;

				vertIndex += 4;
			}

			if (blockClamp > GetBlockHeight(heightMap, blockSize, x, y + 1)) {
				meshData->Vertices[vertIndex] = FVector(-faceSize + facePosition.X, -faceSize + facePosition.Y, blockClamp);
				meshData->Vertices[vertIndex + 1] = FVector(-faceSize + facePosition.X, -faceSize + facePosition.Y, GetBlockHeight(heightMap, blockSize, x, y + 1));
				meshData->Vertices[vertIndex + 2] = FVector(-faceSize + facePosition.X, faceSize + facePosition.Y, blockClamp);
				meshData->Vertices[vertIndex + 3] = FVector(-faceSize + facePosition.X, faceSize + facePosition.Y, GetBlockHeight(heightMap, blockSize, x, y + 1));

				meshData->AddTriangles(vertIndex);

				meshData->SetNormals(vertIndex);

				FVector2D uvScale = FVector2D((1.f / verticesPerLine), (1.f / verticesPerLine));
				FVector2D uvPos = FVector2D(x, y) * uvScale;

				meshData->UVs[vertIndex] = FVector2D(0.f, 1.f) * uvScale + uvPos;
				meshData->UVs[vertIndex + 1] = FVector2D(1.f, 1.f) * uvScale + uvPos;
				meshData->UVs[vertIndex + 2] = FVector2D(0.f, 0.f) * uvScale + uvPos;
				meshData->UVs[vertIndex + 3] = FVector2D(1.f, 0.f) * uvScale + uvPos;

				vertIndex += 4;
			}

			if (blockClamp > GetBlockHeight(heightMap, blockSize, x + 1, y + 2)) {
				meshData->Vertices[vertIndex] = FVector(-faceSize + facePosition.X, faceSize + facePosition.Y, blockClamp);
				meshData->Vertices[vertIndex + 1] = FVector(-faceSize + facePosition.X, faceSize + facePosition.Y, GetBlockHeight(heightMap, blockSize, x + 1, y + 2));
				meshData->Vertices[vertIndex + 2] = FVector(faceSize + facePosition.X, faceSize + facePosition.Y, blockClamp);
				meshData->Vertices[vertIndex + 3] = FVector(faceSize + facePosition.X, faceSize + facePosition.Y, GetBlockHeight(heightMap, blockSize, x + 1, y + 2));

				meshData->AddTriangles(vertIndex);

				meshData->SetNormals(vertIndex);

				FVector2D uvScale = FVector2D((1.f / verticesPerLine), (1.f / verticesPerLine));
				FVector2D uvPos = FVector2D(x, y) * uvScale;

				meshData->UVs[vertIndex] = FVector2D(0.f, 1.f) * uvScale + uvPos;
				meshData->UVs[vertIndex + 1] = FVector2D(1.f, 1.f) * uvScale + uvPos;
				meshData->UVs[vertIndex + 2] = FVector2D(0.f, 0.f) * uvScale + uvPos;
				meshData->UVs[vertIndex + 3] = FVector2D(1.f, 0.f) * uvScale + uvPos;

				vertIndex += 4;
			}

			if (blockClamp > GetBlockHeight(heightMap, blockSize, x + 1, y)) {
				meshData->Vertices[vertIndex] = FVector(faceSize + facePosition.X, -faceSize + facePosition.Y, blockClamp);
				meshData->Vertices[vertIndex + 1] = FVector(faceSize + facePosition.X, -faceSize + facePosition.Y, GetBlockHeight(heightMap, blockSize, x + 1, y));
				meshData->Vertices[vertIndex + 2] = FVector(-faceSize + facePosition.X, -faceSize + facePosition.Y, blockClamp);
				meshData->Vertices[vertIndex + 3] = FVector(-faceSize + facePosition.X, -faceSize + facePosition.Y, GetBlockHeight(heightMap, blockSize, x + 1, y));

				meshData->AddTriangles(vertIndex);

				meshData->SetNormals(vertIndex);

				FVector2D uvScale = FVector2D((1.f / verticesPerLine), (1.f / verticesPerLine));
				FVector2D uvPos = FVector2D(x, y) * uvScale;

				meshData->UVs[vertIndex] = FVector2D(0.f, 1.f) * uvScale + uvPos;
				meshData->UVs[vertIndex + 1] = FVector2D(1.f, 1.f) * uvScale + uvPos;
				meshData->UVs[vertIndex + 2] = FVector2D(0.f, 0.f) * uvScale + uvPos;
				meshData->UVs[vertIndex + 3] = FVector2D(1.f, 0.f) * uvScale + uvPos;

				vertIndex += 4;
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