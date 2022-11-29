#include "MeshSettings_Tri.h"

TArray<int> UMeshSettings_Tri::SupportedChunkSizes;

UMeshSettings_Tri::UMeshSettings_Tri()
{
	SupportedChunkSizes.SetNum(9);
	SupportedChunkSizes[0] = 48;
	SupportedChunkSizes[1] = 72;
	SupportedChunkSizes[2] = 96;
	SupportedChunkSizes[3] = 120;
	SupportedChunkSizes[4] = 144;
	SupportedChunkSizes[5] = 168;
	SupportedChunkSizes[6] = 192;
	SupportedChunkSizes[7] = 216;
	SupportedChunkSizes[8] = 240;
}

int UMeshSettings_Tri::GetNumberVerticesPerLine()
{
	return SupportedChunkSizes[chunkSizeIndex] + 5;
}

float UMeshSettings_Tri::GetMeshWorldSize()
{
	return (GetNumberVerticesPerLine() - 3) * meshScale;
}
