#include "MeshSettings_Block.h"

const int UMeshSettings_Block::SupportedChunkSizes[9]{ 48,72,96,120,144,168,192,216,240 };

float UMeshSettings_Block::GetMeshWorldScale()
{
	return MeshScale * GetChunkSize() * 2.f;
}

float UMeshSettings_Block::GetMeshVertsNum()
{
	return GetChunkSize() + 2;
}

float UMeshSettings_Block::GetChunkSize()
{
	return SupportedChunkSizes[ChunkSizeIndex];
}