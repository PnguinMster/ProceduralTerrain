#pragma once

#include "CoreMinimal.h"
#include "TriMeshSettings.h"
#include "TriMeshGenerator.h"
#include "GameFramework/Actor.h"
#include "TriHeightMapSettings.h"
#include "ProceduralMeshComponent.h"
#include "TriMapThreading.generated.h"

//DECLARE_DELEGATE_OneParam(FMapReceived, UTri_HeightMap*);
//DECLARE_DELEGATE_OneParam(FMeshReceived, UTriMeshData*);
DECLARE_DELEGATE_OneParam(FDataRecieved, UObject*);

UENUM(BlueprintType)
enum class ETriDrawMode : uint8 {
	NoiseMap UMETA(DisplayName = "Noise Map"),
	ColorMap  UMETA(DisplayName = "Color Map"),
};

USTRUCT()
struct FThreadInfo
{
	GENERATED_BODY()

	FDataRecieved* callback;
	UObject* parameter;
	FThreadInfo() {};
	FThreadInfo(FDataRecieved* callback, UObject* parameter) {
		this->callback = callback;
		this->parameter = parameter;
	}
};

UCLASS()
class PROCEDURALTERRAIN_API UTri_HeightMap : public UObject
{
	GENERATED_BODY()

public:
	TArray<TArray<float>> values;
	float minValue;
	float maxValue;

	UTri_HeightMap() {};
	void Initialize(TArray<TArray<float>> Values, float MinValue, float MaxValue) {
		values = Values;
		minValue = MinValue;
		maxValue = MaxValue;
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

	//TQueue<FMapThreadInfo<UTri_HeightMap*>> mapDataThreadInfoQueue;
	//TQueue<FMapThreadInfo<UTriMeshData*>> meshDataThreadInfoQueue;
	TQueue<FThreadInfo> dataQueue;
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

	void RequestData(TFunction<UObject* (void)> generateData, FDataRecieved* callback);
};