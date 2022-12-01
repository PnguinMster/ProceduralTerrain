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

	if (DataQueue.IsEmpty())
		return;

	FThreadInfo_Block threadInfo;
	DataQueue.Dequeue(threadInfo);
	threadInfo.Callback->Execute(threadInfo.Parameter);
}

void UMapThreading_Block::RequestData(TFunction<UObject* (void)> generateData, FDataRecieved* callback)
{
	AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [generateData, callback, this]() {
		UObject* data = generateData();
	AsyncTask(ENamedThreads::GameThread, [callback, data, this]() {
		DataQueue.Enqueue(FThreadInfo_Block(callback, data));
		});
		});
}