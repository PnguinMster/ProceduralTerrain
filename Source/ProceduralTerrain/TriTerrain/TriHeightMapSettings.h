#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TriHeightMapSettings.generated.h"

UCLASS()
class PROCEDURALTERRAIN_API UTriHeightMapSettings : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.01", UIMin = "0.01"))
		float noiseScale = 30.f;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", UIMin = "0.0"))
		int octaves = 4;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float persistance = 0.5f;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1.0", UIMin = "1.0"))
		float lacunarity = 1.f;

	UPROPERTY(EditDefaultsOnly)
		int seed;
	UPROPERTY(EditDefaultsOnly)
		FVector2D offset;
};
