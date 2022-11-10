#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TriMeshSettings.generated.h"

UCLASS()
class PROCEDURALTERRAIN_API UTriMeshSettings : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
		float unformScale = 10.f;
	UPROPERTY(EditDefaultsOnly)
		float meshHeightMultiplier = 5.f;
	UPROPERTY(EditDefaultsOnly)
		UCurveFloat* meshHeightCurve;

	float GetMinHeight();
	float GetMaxHeight();
};
