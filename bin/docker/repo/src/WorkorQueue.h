#pragma once

#include <mutex>
#include<condition_variable>
#include<queue>
#include<memory>
//#include "AbstractOperation.h"
template<typename T>
class threadsafe_queue
{
private:
	mutable std::mutex mut;
	std::queue<std::shared_ptr<T> > data_queue;
	std::condition_variable data_cond;
public:
	threadsafe_queue()
	{}
	void wait_and_pop(T& value)
	{
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk, [this] {return !data_queue.empty(); });
		value = std::move(*data_queue.front());
		data_queue.pop();
	}
	bool try_pop(T& value)
	{
		std::lock_guard<std::mutex> lk(mut);
		if (data_queue.empty())
			return false;
		value = std::move(*data_queue.front());
		data_queue.pop();
		return true;
	}
	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk, [this] {return !data_queue.empty(); });
		std::shared_ptr<T> res = data_queue.front();
		data_queue.pop();
		return res;
	}

	std::shared_ptr<T> try_pop()
	{
		std::lock_guard<std::mutex> lk(mut);
		if (data_queue.empty())
			return std::shared_ptr<T>();
		std::shared_ptr<T> res = data_queue.front();
		data_queue.pop();
		return res;
	}
	void push(T new_value)
	{
		std::shared_ptr<T> data(
			std::make_shared<T>(std::move(new_value)));
		std::lock_guard<std::mutex> lk(mut);
		data_queue.push(data);
		data_cond.notify_one();
	}
	bool empty() const
	{
		std::lock_guard<std::mutex> lk(mut);
		return data_queue.empty();
	}
};

template<class T>
class WorkorQueue
{
public:
	WorkorQueue()
	{};
	~WorkorQueue()
	{};

	//void Push(std::shared_ptr<T> op)
	void Push(T op)
	{
		std::lock_guard<std::mutex> lk(mutex_);
		queue_.push(std::move(op));
		//LOG(INFO) << "Size of queue is " << queue_.size();
		condition_.notify_all();
	}
	
	//void Push(T &&op)
	//{
	//	std::lock_guard<std::mutex> lk(mutex_);
	//	queue_.push(std::move(op));
	//	//LOG(INFO) << "Size of queue is " << queue_.size();
	//	condition_.notify_all();
	//}

	//std::shared_ptr<T> Pop()
	T Pop()
	{
		std::unique_lock<std::mutex> lk(mutex_);
		//LOG(INFO) << "Size of queue is " << queue_.size();
		condition_.wait(lk, [&]() { return !queue_.empty(); });
		auto op = queue_.front();
		queue_.pop();
		return op;
	}

private:
	std::mutex mutex_;
	std::condition_variable condition_;
	//std::queue<std::shared_ptr<T>> queue_;
	std::queue<T> queue_;
private:

};



