#pragma once

#include "CoreMinimal.h"
#include "TriMeshSettings.h"
#include "TriMeshGenerator.h"
#include "GameFramework/Actor.h"
#include "TriHeightMapSettings.h"
#include "ProceduralMeshComponent.h"
#include "TriMapThreading.generated.h"

DECLARE_DELEGATE_OneParam(FMapReceived, FTri_HeightMap);
DECLARE_DELEGATE_OneParam(FMeshReceived, UTriMeshData*);

UENUM(BlueprintType)
enum class ETriDrawMode : uint8 {
	NoiseMap UMETA(DisplayName = "Noise Map"),
	ColorMap  UMETA(DisplayName = "Color Map"),
};

template<typename T>
struct FMapThreadInfo
{
	TDelegate<void(T)>* callback;
	T parameter;
	FMapThreadInfo() {};
	FMapThreadInfo(TDelegate<void(T)>* callback, T parameter) {
		this->callback = callback;
		this->parameter = parameter;
	}
};

USTRUCT()
struct FTri_HeightMap
{
	GENERATED_BODY()

public:
	TArray<TArray<float>> values;
	float minValue;
	float maxValue;

	FTri_HeightMap() {};
	FTri_HeightMap(TArray<TArray<float>> values, float minValue, float maxValue) {
		this->values = values;
		this->minValue = minValue;
		this->maxValue = maxValue;
	}
};

USTRUCT()
struct FTriTerrainType {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float height;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float blend;
	UPROPERTY(EditAnywhere)
		FColor color;
};

UCLASS()
class PROCEDURALTERRAIN_API ATriMapThreading : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditDefaultsOnly)
		UTriHeightMapSettings* heightMapSettings;
	UPROPERTY(EditDefaultsOnly)
		UTriMeshSettings* meshSettings;

	UPROPERTY(EditDefaultsOnly)
		TArray<FTriTerrainType> regions;
	//UPROPERTY(EditDefaultsOnly)
		//UMaterial* chunkMaterial;
	UPROPERTY(EditDefaultsOnly)
		ETriDrawMode drawMode = ETriDrawMode::ColorMap;

	TQueue<FMapThreadInfo<FTri_HeightMap>> mapDataThreadInfoQueue;
	TQueue<FMapThreadInfo<UTriMeshData*>> meshDataThreadInfoQueue;

	// Sets default values for this actor's properties
	ATriMapThreading();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UProceduralMeshComponent* terrainMesh;
	UMaterialInterface* dynamicMaterial;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DrawMapInEditor();

	void RequestHeightData(FVector2D center, FMapReceived* callback);
	void RequestMeshData(FTri_HeightMap heightMap, int lod, FMeshReceived* callback);
};