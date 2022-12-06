#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapThreading_Tri.generated.h"

DECLARE_DELEGATE_OneParam(FDataRecieved, UObject*);

USTRUCT()
struct FThreadInfo_Tri
{
	GENERATED_BODY()

	FDataRecieved* Callback = nullptr;
	UObject* Parameter = nullptr;

	FThreadInfo_Tri() {};
	FThreadInfo_Tri(FDataRecieved* callback, UObject* parameter);
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROCEDURALTERRAIN_API UMapThreading_Tri : public UActorComponent
{
	GENERATED_BODY()

public:
	UMapThreading_Tri();

protected:
	TQueue<FThreadInfo_Tri> DataQueue;
	static int ChunkIterationPerTick;

	virtual void TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;
	void IterateQueue();
public:
	void RequestData(TFunction<UObject* (void)> generateData, FDataRecieved* callback);
};