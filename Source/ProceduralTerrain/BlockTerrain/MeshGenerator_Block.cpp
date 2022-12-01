#include "MeshGenerator_Block.h"

UMeshData_Block* MeshGenerator_Block::GenerateTerrainMesh(TArray<TArray<float>> heightMap, int blockSize, int levelOfDetail)
{
	UMeshData_Block* meshData = NewObject<UMeshData_Block>();
	int borderSize = heightMap.Num();
	int meshSize = borderSize - 2;

	int meshSimplificationIncrement = (levelOfDetail == 0) ? 1 : levelOfDetail * 2;
	int verticesPerLine = (meshSize - 1) / meshSimplificationIncrement + 1;
	meshData->SetArraysSize(FMath::Square(verticesPerLine) * 4);

	float topLeftX = ((meshSize - 1) * -blockSize) + (blockSize * meshSimplificationIncrement);
	float topLeftY = ((meshSize - 1) * -blockSize) + (blockSize * meshSimplificationIncrement);

	float faceSize = blockSize * meshSimplificationIncrement;
	float spacing = blockSize * 2.f;
	int vertIndex = 0;

	//***********************
	//
	// Add Initial Vertices
	//
	// **********************
	//Add Vertices UVs for top face
	for (int y = 0; y < meshSize; y += meshSimplificationIncrement) {
		for (int x = 0; x < meshSize; x += meshSimplificationIncrement) {
			//Calculate Face Values
			FVector2D facePosition = FVector2D(spacing * x + topLeftX, spacing * y + topLeftY);
			int blockClamp = GetBlockHeight(heightMap, blockSize, x + 1, y + 1);

			//Set Vertex Points
			meshData->AddBlockVertices(vertIndex, faceSize, facePosition, blockClamp);
			meshData->SetNormals(vertIndex);
			meshData->AddUVS(vertIndex, x, y, verticesPerLine);

			vertIndex += 4;
		}
	}

	//Temporary Variables
	FVector DefinedVertex;
	FVector2D DefinedUV;
	int triIndex = 0;
	vertIndex = 0;
	int vertexCount = meshData->Vertices.Num();

	//******************************
	//
	//Add Side Vertices and Faces
	//
	//******************************
	while (triIndex < vertexCount) {
		//TOP FACE
		meshData->AddTriangles(triIndex);

		//Create Tangents
		for (int i = 0; i < 4; i++)
			meshData->Tangents.Add(FProcMeshTangent(0.f, 0.f, 1.f));

		//****************
		//
		//X SIDE FACES
		//
		//****************
		if (triIndex < (verticesPerLine * verticesPerLine * 4) - 4 && meshData->Vertices[triIndex + 3].Z != meshData->Vertices[triIndex + 4].Z
			&& (triIndex + 4) % (verticesPerLine * 4) != 0) {
			//Create X Side Face Vertices
			meshData->Vertices.Add(DefinedVertex = meshData->Vertices[triIndex + 1]);
			meshData->Vertices.Add(DefinedVertex = meshData->Vertices[triIndex + 4]);
			meshData->Vertices.Add(DefinedVertex = meshData->Vertices[triIndex + 3]);
			meshData->Vertices.Add(DefinedVertex = meshData->Vertices[triIndex + 6]);

			//Create X Side Face
			meshData->AddTriangles(vertexCount + vertIndex);

			vertIndex += 4; // Vertex Index Count
			//Left Face
			if (meshData->Vertices[triIndex + 3].Z < meshData->Vertices[triIndex + 4].Z) {
				//Add Appropriate UVs
				meshData->UVs.Add(DefinedUV = meshData->UVs[triIndex + 4]);
				meshData->UVs.Add(DefinedUV = meshData->UVs[triIndex + 6]);
				meshData->UVs.Add(DefinedUV = meshData->UVs[triIndex + 5]);
				meshData->UVs.Add(DefinedUV = meshData->UVs[triIndex + 7]);

				//Calculate Face Normal and Add Tangent
				FVector faceNormal = FVector::CrossProduct(meshData->Vertices[triIndex + 3] - meshData->Vertices[triIndex + 1], meshData->Vertices[triIndex + 6] - meshData->Vertices[triIndex + 1]).GetSafeNormal();
				for (int i = 0; i < 4; i++) {
					meshData->Tangents.Add(FProcMeshTangent(-1.f, 0.f, 0.f));
					meshData->Normals.Add(faceNormal);
				}
			}
			//Right Face
			else {
				//Add Appropriate UVs
				meshData->AddExistingUVs(triIndex);

				//Calculate Face Normal and Add Tangent
				FVector faceNormal = FVector::CrossProduct(meshData->Vertices[triIndex + 3] - meshData->Vertices[triIndex + 1], meshData->Vertices[triIndex + 6] - meshData->Vertices[triIndex + 1]).GetSafeNormal();
				for (int i = 0; i < 4; i++) {
					meshData->Tangents.Add(FProcMeshTangent(1.f, 0.f, 0.f));
					meshData->Normals.Add(faceNormal);
				}
			}
		}
		//*************
		//
		//Y SIDE FACES
		//
		//*************

		if (triIndex < (verticesPerLine * verticesPerLine * 4) - (verticesPerLine * 4) &&
			meshData->Vertices[triIndex].Z != meshData->Vertices[triIndex + (verticesPerLine * 4)].Z) {
			//Create Y side Vertices
			meshData->Vertices.Add(DefinedVertex = meshData->Vertices[triIndex]);
			meshData->Vertices.Add(DefinedVertex = meshData->Vertices[triIndex + (verticesPerLine * 4) + 2]);
			meshData->Vertices.Add(DefinedVertex = meshData->Vertices[triIndex + 1]);
			meshData->Vertices.Add(DefinedVertex = meshData->Vertices[triIndex + (verticesPerLine * 4) + 3]);

			//Create Y side
			meshData->AddTriangles(vertexCount + vertIndex);

			vertIndex += 4; // Vertex Index Count
			//Front Face
			if (meshData->Vertices[triIndex].Z < meshData->Vertices[triIndex + (verticesPerLine * 4)].Z) {
				//Add Appropriate UVs
				meshData->AddExistingUVs(triIndex + (verticesPerLine * 4));

				//Calculate Face Normal and Add Tangent
				FVector faceNormal = FVector::CrossProduct(meshData->Vertices[triIndex + 1] - meshData->Vertices[triIndex], meshData->Vertices[triIndex + (verticesPerLine * 4) + 3] - meshData->Vertices[triIndex]).GetSafeNormal();
				for (int i = 0; i < 4; i++) {
					meshData->Tangents.Add(FProcMeshTangent(0.f, 1.f, 0.f));
					meshData->Normals.Add(faceNormal);
				}
			}
			//Back Face
			else {
				//Add Appropriate UVs
				meshData->AddExistingUVs(triIndex);

				//Calculate Face Normal and Add Tangent
				FVector faceNormal = FVector::CrossProduct(meshData->Vertices[triIndex + 1] - meshData->Vertices[triIndex], meshData->Vertices[triIndex + (verticesPerLine * 4) + 3] - meshData->Vertices[triIndex]).GetSafeNormal();
				for (int i = 0; i < 4; i++) {
					meshData->Tangents.Add(FProcMeshTangent(0.f, -1.f, 0.f));
					meshData->Normals.Add(faceNormal);
				}
			}
		}
		triIndex += 4; // Vertex Index Count
	}

	/*************
	*
	* Adds Border faces and vertices
	* to transition between chunks
	*
	*************/
	vertIndex = 0;
	int newVertexCount = meshData->Vertices.Num();

	for (int i = 0; i < verticesPerLine; i++) {
		//************************
		//
		// X Border
		//
		//***********************
		//Get block height for X border
		int blockClampX = GetBlockHeight(heightMap, blockSize, i + 1, verticesPerLine + 1);
		//Get X border index
		int currentXIndex = ((verticesPerLine - 1) * verticesPerLine + i) * 4;

		if (meshData->Vertices[currentXIndex].Z > blockClampX) {
			//Add Vertices
			meshData->Vertices.Add(FVector(meshData->Vertices[currentXIndex].X, meshData->Vertices[currentXIndex].Y, meshData->Vertices[currentXIndex].Z));
			meshData->Vertices.Add(FVector(meshData->Vertices[currentXIndex].X, meshData->Vertices[currentXIndex].Y, blockClampX));
			meshData->Vertices.Add(FVector(meshData->Vertices[currentXIndex + 1].X, meshData->Vertices[currentXIndex + 1].Y, meshData->Vertices[currentXIndex + 1].Z));
			meshData->Vertices.Add(FVector(meshData->Vertices[currentXIndex + 1].X, meshData->Vertices[currentXIndex + 1].Y, blockClampX));

			//Add triangles
			meshData->AddTriangles(newVertexCount + vertIndex);

			//Add UVs
			meshData->AddExistingUVs(currentXIndex);

			//Calculate normals
			FVector vertexNormal = FVector::CrossProduct(meshData->Vertices[newVertexCount + vertIndex + 2] - meshData->Vertices[newVertexCount + vertIndex + 1], meshData->Vertices[newVertexCount + vertIndex + 2] - meshData->Vertices[newVertexCount + vertIndex]).GetSafeNormal();

			//Add Normals and tangents
			for (int n = 0; n < 4; n++) {
				meshData->Normals.Add(vertexNormal);
				meshData->Tangents.Add(FProcMeshTangent(1.f, 0.f, 0.f));
			}
			vertIndex += 4;
		}

		//Get block height for X border
		blockClampX = GetBlockHeight(heightMap, blockSize, i + 1, 0);
		//Get X border index
		currentXIndex = i * 4;

		if (meshData->Vertices[currentXIndex].Z > blockClampX) {
			//Add Vertices
			meshData->Vertices.Add(FVector(meshData->Vertices[currentXIndex + 3].X, meshData->Vertices[currentXIndex + 3].Y, meshData->Vertices[currentXIndex + 3].Z));
			meshData->Vertices.Add(FVector(meshData->Vertices[currentXIndex + 3].X, meshData->Vertices[currentXIndex + 3].Y, blockClampX));
			meshData->Vertices.Add(FVector(meshData->Vertices[currentXIndex + 2].X, meshData->Vertices[currentXIndex + 2].Y, meshData->Vertices[currentXIndex + 2].Z));
			meshData->Vertices.Add(FVector(meshData->Vertices[currentXIndex + 2].X, meshData->Vertices[currentXIndex + 2].Y, blockClampX));

			//Add triangles
			meshData->AddTriangles(newVertexCount + vertIndex);

			//Add UVs
			meshData->AddExistingUVs(currentXIndex);

			//Calculate normals
			FVector vertexNormal = FVector::CrossProduct(meshData->Vertices[newVertexCount + vertIndex + 2] - meshData->Vertices[newVertexCount + vertIndex + 1], meshData->Vertices[newVertexCount + vertIndex + 2] - meshData->Vertices[newVertexCount + vertIndex]).GetSafeNormal();

			//Add Normals and tangents
			for (int n = 0; n < 4; n++) {
				meshData->Normals.Add(vertexNormal);
				meshData->Tangents.Add(FProcMeshTangent(1.f, 0.f, 0.f));
			}
			vertIndex += 4;
		}

		//************************
		//
		// Y Border
		//
		// ***********************
		//Get block height for Y border
		int blockClampY = GetBlockHeight(heightMap, blockSize, verticesPerLine + 1, i + 1);
		//Get Y border index
		int currentYIndex = (i * verticesPerLine + verticesPerLine - 1) * 4;

		if (meshData->Vertices[currentYIndex].Z > blockClampY) {
			//Add vertices
			meshData->Vertices.Add(FVector(meshData->Vertices[currentYIndex + 1].X, meshData->Vertices[currentYIndex + 1].Y, meshData->Vertices[currentYIndex + 1].Z));
			meshData->Vertices.Add(FVector(meshData->Vertices[currentYIndex + 1].X, meshData->Vertices[currentYIndex + 1].Y, blockClampY));
			meshData->Vertices.Add(FVector(meshData->Vertices[currentYIndex + 3].X, meshData->Vertices[currentYIndex + 3].Y, meshData->Vertices[currentYIndex + 3].Z));
			meshData->Vertices.Add(FVector(meshData->Vertices[currentYIndex + 3].X, meshData->Vertices[currentYIndex + 3].Y, blockClampY));

			//Add Triangles
			meshData->AddTriangles(newVertexCount + vertIndex);

			//Add UVs
			meshData->AddExistingUVs(currentYIndex);

			//Calculate normals
			FVector vertexNormal = FVector::CrossProduct(meshData->Vertices[newVertexCount + vertIndex + 2] - meshData->Vertices[newVertexCount + vertIndex + 1], meshData->Vertices[newVertexCount + vertIndex + 2] - meshData->Vertices[newVertexCount + vertIndex]).GetSafeNormal();

			//Add normals and tangents for face
			for (int n = 0; n < 4; n++) {
				meshData->Normals.Add(vertexNormal);
				meshData->Tangents.Add(FProcMeshTangent(0.f, 1.f, 0.f));
			}

			vertIndex += 4;
		}

		//Get block height for Y border
		blockClampY = GetBlockHeight(heightMap, blockSize, 0, i + 1);
		//Get Y border index
		currentYIndex = (i * verticesPerLine) * 4;

		if (meshData->Vertices[currentYIndex].Z > blockClampY) {
			//Add vertices
			meshData->Vertices.Add(FVector(meshData->Vertices[currentYIndex + 2].X, meshData->Vertices[currentYIndex + 2].Y, meshData->Vertices[currentYIndex + 2].Z));
			meshData->Vertices.Add(FVector(meshData->Vertices[currentYIndex + 2].X, meshData->Vertices[currentYIndex + 2].Y, blockClampY));
			meshData->Vertices.Add(FVector(meshData->Vertices[currentYIndex].X, meshData->Vertices[currentYIndex].Y, meshData->Vertices[currentYIndex].Z));
			meshData->Vertices.Add(FVector(meshData->Vertices[currentYIndex].X, meshData->Vertices[currentYIndex].Y, blockClampY));

			//Add Triangles
			meshData->AddTriangles(newVertexCount + vertIndex);

			//Add UVs
			meshData->AddExistingUVs(currentYIndex);

			//Calculate normals
			FVector vertexNormal = FVector::CrossProduct(meshData->Vertices[newVertexCount + vertIndex + 2] - meshData->Vertices[newVertexCount + vertIndex + 1], meshData->Vertices[newVertexCount + vertIndex + 2] - meshData->Vertices[newVertexCount + vertIndex]).GetSafeNormal();

			//Add normals and tangents for face
			for (int n = 0; n < 4; n++) {
				meshData->Normals.Add(vertexNormal);
				meshData->Tangents.Add(FProcMeshTangent(0.f, 1.f, 0.f));
			}

			vertIndex += 4;
		}
	}

	return meshData;
}

int MeshGenerator_Block::GetBlockHeight(TArray<TArray<float>>& heightMap, int blockSize, int x, int y)
{
	//Clamp to block size
	int blockClamp = blockSize + (heightMap[x][y] * blockSize);
	blockClamp -= (blockClamp % (blockSize * 2));

	return blockClamp;
}