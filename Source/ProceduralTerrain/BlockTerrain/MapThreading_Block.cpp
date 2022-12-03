#include "MapThreading_Block.h"
#include "Async/Async.h"

FThreadInfo_Block::FThreadInfo_Block(FDataRecieved* callback, UObject* parameter)
{
	Callback = callback;
	Parameter = parameter;
}

UMapThreading_Block::UMapThreading_Block()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMapThreading_Block::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	if (IsInitialChunks) {
		while (!DataQueue.IsEmpty()) IterateQueue();
		IsInitialChunks = false;
	}

	if (!DataQueue.IsEmpty()) IterateQueue();
}

void UMapThreading_Block::IterateQueue()
{
	FThreadInfo_Block threadInfo;
	DataQueue.Dequeue(threadInfo);
	threadInfo.Callback->Execute(threadInfo.Parameter);
}

void UMapThreading_Block::RequestData(TFunction<UObject* (void)> generateData, FDataRecieved* callback)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [generateData, callback, this]() {
		UObject* data = generateData();
	AsyncTask(ENamedThreads::GameThread, [callback, data, this]() {
		DataQueue.Enqueue(FThreadInfo_Block(callback, data));
		});
		});
}