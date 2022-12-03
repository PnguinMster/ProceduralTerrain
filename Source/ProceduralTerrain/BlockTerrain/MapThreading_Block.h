#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapThreading_Block.generated.h"

DECLARE_DELEGATE_OneParam(FDataRecieved, UObject*);

USTRUCT()
struct FThreadInfo_Block
{
	GENERATED_BODY()

		FDataRecieved* Callback = nullptr;
	UObject* Parameter = nullptr;

	FThreadInfo_Block() {};
	FThreadInfo_Block(FDataRecieved* callback, UObject* parameter);
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROCEDURALTERRAIN_API UMapThreading_Block : public UActorComponent
{
	GENERATED_BODY()
public:
	UMapThreading_Block();
	void RequestData(TFunction<UObject* (void)> generateData, FDataRecieved* callback);

protected:
	TQueue<FThreadInfo_Block> DataQueue;
	bool IsInitialChunks;

	virtual void TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;
	void IterateQueue();
};