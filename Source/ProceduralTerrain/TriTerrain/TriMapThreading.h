#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TriMapThreading.generated.h"

DECLARE_DELEGATE_OneParam(FDataRecieved, UObject*);

USTRUCT()
struct FThreadInfo
{
	GENERATED_BODY()

	FDataRecieved* callback = nullptr;
	UObject* parameter = nullptr;

	FThreadInfo() {};
	FThreadInfo(FDataRecieved* callback, UObject* parameter);
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROCEDURALTERRAIN_API UTriMapThreading : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UTriMapThreading();

protected:
	TQueue<FThreadInfo> dataQueue;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RequestData(TFunction<UObject* (void)> generateData, FDataRecieved* callback);
};