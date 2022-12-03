#include "MeshSettings_Block.h"

TArray<int> UMeshSettings_Block::SupportedChunkSizes;

UMeshSettings_Block::UMeshSettings_Block()
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
