#include "MapThreading.h"
#include "Async/Async.h"

FThreadInfo::FThreadInfo(FDataRecieved* callback, UObject* parameter)
{
	Callback = callback;
	Parameter = parameter;
}

int UMapThreading::ChunkIterationPerTick = 3;

UMapThreading::UMapThreading()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMapThreading::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	int iterator = 0;
	while (!DataQueue.IsEmpty()) {
		if (iterator >= ChunkIterationPerTick)
			return;
		IterateQueue();
		iterator++;
	}
}

void UMapThreading::IterateQueue()
{
	FThreadInfo threadInfo;
	DataQueue.Dequeue(threadInfo);
	threadInfo.Callback->Execute(threadInfo.Parameter);
}

void UMapThreading::RequestData(TFunction<UObject* (void)> generateData, FDataRecieved* callback)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [generateData, callback, this]() {
		UObject* data = generateData();
		AsyncTask(ENamedThreads::GameThread, [callback, data, this]() {
			DataQueue.Enqueue(FThreadInfo(callback, data));
			});
		});
}
