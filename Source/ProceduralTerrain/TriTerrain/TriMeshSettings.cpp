#include "TriMeshSettings.h"

float UTriMeshSettings::GetMeshWorldSize()
{
	return (numVertsPerLine - 3) * meshScale;
}
