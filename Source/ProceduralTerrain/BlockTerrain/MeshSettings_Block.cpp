#include "MeshSettings_Block.h"

const int UMeshSettings_Block::SupportedChunkSizes[9]{ 48,72,96,120,144,168,192,216,240 };

float UMeshSettings_Block::GetMeshWorldSize()
{
	return (GetMeshVertsNum() - 2) * MeshScale;
}

float UMeshSettings_Block::GetMeshVertsNum()
{
	return SupportedChunkSizes[ChunkSizeIndex] + 2;
}
