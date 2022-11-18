#include "TriMapThreading.h"
#include "Async/Async.h"

FThreadInfo::FThreadInfo(FDataRecieved* callback, UObject* parameter)
{
	this->callback = callback;
	this->parameter = parameter;
}


ATriMapThreading::ATriMapThreading()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATriMapThreading::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!dataQueue.IsEmpty()) {
		FThreadInfo threadInfo;
		dataQueue.Dequeue(threadInfo);
		threadInfo.callback->Execute(threadInfo.parameter);
	}
}

void ATriMapThreading::RequestData(TFunction<UObject* (void)> generateData, FDataRecieved* callback)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [generateData, callback, this]() {
		UObject* data = generateData();
		AsyncTask(ENamedThreads::GameThread, [callback, data, this]() {
			dataQueue.Enqueue(FThreadInfo(callback, data));
		});
	});
}