#pragma once

#include <mutex>
#include<condition_variable>
#include<queue>
#include<memory>
#include "AbstractOperation.h"
class WorkorQueue
{
public:
	WorkorQueue();
	~WorkorQueue();

	void Push(std::shared_ptr<AbstractOperation> msg);
	std::shared_ptr<AbstractOperation> Pop();

private:
	std::mutex mutex_;
	std::condition_variable condition_;
	std::queue<std::shared_ptr<AbstractOperation>> queue_;
private:

};



