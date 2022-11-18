#pragma once

#include "CoreMinimal.h"
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

UCLASS()
class PROCEDURALTERRAIN_API ATriMapThreading : public AActor
{
	GENERATED_BODY()
	
public:	
	ATriMapThreading();

protected:
	TQueue<FThreadInfo> dataQueue;

public:	
	virtual void Tick(float DeltaTime) override;
	void RequestData(TFunction<UObject* (void)> generateData, FDataRecieved* callback);
};