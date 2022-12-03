#include "MapThreading_Tri.h"
#include "Async/Async.h"

FThreadInfo_Tri::FThreadInfo_Tri(FDataRecieved* callback, UObject* parameter)
{
	Callback = callback;
	Parameter = parameter;
}

UMapThreading_Tri::UMapThreading_Tri()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMapThreading_Tri::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	if (IsInitialChunks) {
		while (!DataQueue.IsEmpty()) IterateQueue();
		IsInitialChunks = false;
	}

	if (!DataQueue.IsEmpty()) IterateQueue();
}

void UMapThreading_Tri::IterateQueue()
{
	FThreadInfo_Tri threadInfo;
	DataQueue.Dequeue(threadInfo);
	threadInfo.Callback->Execute(threadInfo.Parameter);
}

void UMapThreading_Tri::RequestData(TFunction<UObject* (void)> generateData, FDataRecieved* callback)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [generateData, callback, this]() {
		UObject* data = generateData();
		AsyncTask(ENamedThreads::GameThread, [callback, data, this]() {
			DataQueue.Enqueue(FThreadInfo_Tri(callback, data));
		});
	});
}