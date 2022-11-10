#pragma once

#include "CoreMinimal.h"
#include "TriMeshSettings.h"
#include "TriMeshGenerator.h"
#include "GameFramework/Actor.h"
#include "TriHeightMapSettings.h"
#include "ProceduralMeshComponent.h"
#include "TriMapThreading.generated.h"

DECLARE_DELEGATE_OneParam(FMapReceived, FMapData);
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
struct FMapData
{
	GENERATED_BODY()

public:
	TArray<TArray<float>> noiseMap;
	//TArray<uint8> color;

	FMapData() {};
	FMapData(TArray<TArray<float>> values) {
		this->noiseMap = values;
		//this->color = color;
	}
};

USTRUCT()
struct FTriTerrainType {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float height;
	UPROPERTY(EditAnywhere)
		FColor color;
};

UCLASS()
class PROCEDURALTERRAIN_API ATriMapThreading : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1.0", UIMin = "1.0"))
		int mapChunkSize = 95;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "6.0", UIMin = "0.0", UIMax = "6.0"))
		int editorPreviewLOD;

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

	TQueue<FMapThreadInfo<FMapData>> mapDataThreadInfoQueue;
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

	FMapData GenerateMapData(FVector2D center);
	void DrawMapInEditor();

	void RequestMapData(FVector2D center, FMapReceived* callback);
	void RequestMeshData(FMapData mapData, int lod, FMeshReceived* callback);
};