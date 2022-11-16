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
		float meshScale = 10.f;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1.0", UIMin = "1.0"))
		int numVertsPerLine = 77;

	float GetMeshWorldSize();
};
