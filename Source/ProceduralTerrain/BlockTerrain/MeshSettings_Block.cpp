#include "MeshSettings_Block.h"

float UMeshSettings_Block::GetMeshWorldScale()
{
	return MeshScale * ChunkSize * 2.f;
}

float UMeshSettings_Block::GetMeshVertsNum()
{
	return ChunkSize + 2;
}
