#include "MeshSettings_Tri.h"

float UMeshSettings_Tri::GetMeshWorldSize()
{
	return (numberOfVerticesPerLine - 3) * meshScale;
}
