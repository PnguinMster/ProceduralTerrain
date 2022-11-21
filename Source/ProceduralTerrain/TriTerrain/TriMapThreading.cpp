#include "TriMapThreading.h"
#include "Async/Async.h"

FThreadInfo::FThreadInfo(FDataRecieved* callback, UObject* parameter)
{
	this->callback = callback;
	this->parameter = parameter;
}


UTriMapThreading::UTriMapThreading()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTriMapThreading::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!dataQueue.IsEmpty()) {
		FThreadInfo threadInfo;
		dataQueue.Dequeue(threadInfo);
		threadInfo.callback->Execute(threadInfo.parameter);
	}
}

void UTriMapThreading::RequestData(TFunction<UObject* (void)> generateData, FDataRecieved* callback)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [generateData, callback, this]() {
		UObject* data = generateData();
		AsyncTask(ENamedThreads::GameThread, [callback, data, this]() {
			dataQueue.Enqueue(FThreadInfo(callback, data));
		});
	});
}