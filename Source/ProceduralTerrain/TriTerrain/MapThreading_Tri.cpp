#include "MapThreading_Tri.h"
#include "Async/Async.h"

FThreadInfo::FThreadInfo(FDataRecieved* callback, UObject* parameter)
{
	this->callback = callback;
	this->parameter = parameter;
}

UMapThreading_Tri::UMapThreading_Tri()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMapThreading_Tri::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (dataQueue.IsEmpty())
		return;

	FThreadInfo threadInfo;
	dataQueue.Dequeue(threadInfo);
	threadInfo.callback->Execute(threadInfo.parameter);
}

void UMapThreading_Tri::RequestData(TFunction<UObject* (void)> generateData, FDataRecieved* callback)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [generateData, callback, this]() {
		UObject* data = generateData();
		AsyncTask(ENamedThreads::GameThread, [callback, data, this]() {
			dataQueue.Enqueue(FThreadInfo(callback, data));
		});
	});
}