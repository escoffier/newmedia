#include "WorkorQueue.h"
#include "glog\logging.h"


WorkorQueue::WorkorQueue()
{
}

WorkorQueue::~WorkorQueue()
{
}

void WorkorQueue::Push(std::shared_ptr<AbstractOperation> op)
{
	std::lock_guard<std::mutex> lk(mutex_);
	queue_.push(op);
	LOG(INFO) <<"Size of queue is "<< queue_.size();
	condition_.notify_all();
}

std::shared_ptr<AbstractOperation> WorkorQueue::Pop()
{
	std::unique_lock<std::mutex> lk(mutex_);
	LOG(INFO) << "Size of queue is " << queue_.size();
	condition_.wait(lk, [&]() { return !queue_.empty(); });
	auto op = queue_.front();
	queue_.pop();
	return op;
}
