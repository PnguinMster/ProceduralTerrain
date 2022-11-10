#include "TriMeshSettings.h"

float UTriMeshSettings::GetMinHeight()
{
	return unformScale * meshHeightMultiplier * meshHeightCurve->GetFloatValue(0);
}

float UTriMeshSettings::GetMaxHeight()
{
	return unformScale * meshHeightMultiplier * meshHeightCurve->GetFloatValue(1);
}
