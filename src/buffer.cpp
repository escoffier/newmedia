#include "buffer.h"
#include "glog/logging.h"

using std::mutex;
Buffer::Buffer()
{
}

Buffer::~Buffer()
{
	// {
	// 	std::lock_guard<mutex> lk(packetMutex_);
	// 	packetQueue_.clear();
	// }

	clearSenders();
}

void Buffer::addSender(const std::string &callid, std::shared_ptr<PaketSender> sender)
{
	std::lock_guard<std::mutex> lk(destsmutex_);
	senders_.emplace(callid, sender);
}

void Buffer::removeSender(const std::string &callid)
{
	std::lock_guard<std::mutex> lk(destsmutex_);
	senders_.erase(callid);
}

void Buffer::operator()()
{
}

void Buffer::clearSenders()
{
	std::lock_guard<std::mutex> lk(destsmutex_);
	senders_.clear();
}

int Buffer::senderNumber()
{
	std::lock_guard<std::mutex> lk(destsmutex_);
	return senders_.size();
}

void Buffer::processData()
{
	std::lock_guard<std::mutex> lk(destsmutex_);
	//LOG(INFO) << "11processData";
	for (auto &sender : senders_)
	{
		//LOG(ERROR) << "processData";
		std::shared_ptr<AVPacket>  pkt = popPacket();
		if(pkt)
		{
			sender.second->send((uint8_t*)pkt->data(), pkt->size());
		}
		//sender.send()
	}
}

std::shared_ptr<AVPacket> Buffer::popPacket()
{
	std::lock_guard<mutex> lk(packetMutex_);
	if(packetQueue_.empty())
		return std::shared_ptr<AVPacket>();
	std::shared_ptr<AVPacket> pkt = packetQueue_.front();
	packetQueue_.pop();
	return pkt;
}

void Buffer::pushPacket(std::shared_ptr<AVPacket> pkt)
{
	std::lock_guard<mutex> lk(packetMutex_);
	packetQueue_.push(pkt);
}