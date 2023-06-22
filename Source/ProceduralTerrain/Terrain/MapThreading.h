#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapThreading.generated.h"

DECLARE_DELEGATE_OneParam(FDataRecieved, UObject*);

USTRUCT()
struct FThreadInfo
{
	GENERATED_BODY()

	FDataRecieved* Callback = nullptr;
	UObject* Parameter = nullptr;

	FThreadInfo() {};
	FThreadInfo(FDataRecieved* callback, UObject* parameter);
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROCEDURALTERRAIN_API UMapThreading : public UActorComponent
{
	GENERATED_BODY()

public:
	UMapThreading();
	void RequestData(TFunction<UObject* (void)> generateData, FDataRecieved* callback);

protected:
	TQueue<FThreadInfo> DataQueue;
	static int ChunkIterationPerTick;

	virtual void TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;
	void IterateQueue();
};
