#include "MeshSettings_Tri.h"

const int UMeshSettings_Tri::SupportedChunkSizes[9]{ 48,72,96,120,144,168,192,216,240 };

int UMeshSettings_Tri::GetNumberVerticesPerLine()
{
	return SupportedChunkSizes[ChunkSizeIndex] + 5;
}

float UMeshSettings_Tri::GetMeshWorldSize()
{
	return (GetNumberVerticesPerLine() - 3) * MeshScale;
}
