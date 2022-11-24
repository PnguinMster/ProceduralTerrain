#include "MeshSettings_Tri.h"

float UMeshSettings_Tri::GetMeshWorldSize()
{
	return (numVertsPerLine - 3) * meshScale;
}
