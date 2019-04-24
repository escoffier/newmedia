#pragma once
#include <cstdint>
#include <queue>
#include <memory>
#include<condition_variable>
#include <iostream>
#include <map>
//#include "RTPSender.h"
#include "jrtplib3/rtpsession.h"
#include "jrtplib3/rtpudpv4transmitter.h"
//#include "jrtplib3\rtpipv4address.h"
//#include "jrtplib3\rtpsessionparams.h"
#include "threadsafequeue.h"
#ifdef _DEBUG
#include<fstream>
#endif // _DEBUG


class PSBuffer/*: public std::enable_shared_from_this<PSBuffer>*/
{
	using RTPSessionPrt = std::unique_ptr<jrtplib::RTPSession>;
public:
	PSBuffer();
	~PSBuffer();
	//std::shared_ptr<PSBuffer> getPtr() { return shared_from_this(); }
	struct Destination
	{
		Destination() : rtpsession_(new jrtplib::RTPSession){}
		Destination(const Destination & other) = delete;
		Destination & operator=(const Destination & other) = delete;
		
		Destination(Destination && other):
			callid_(std::move(other.callid_)), destip_(other.destip_),destport_(other.destport_), ssrc_(other.ssrc_), pt_(other.pt_), rtpsession_(std::move(other.rtpsession_))
		{}
		Destination & operator=(Destination && other); 

		int createRTPSession(uint32_t ssrc, jrtplib::RTPTransmitter* transmitter);

		std::string callid_;
		uint32_t destip_;
		uint32_t destport_;
		uint32_t ssrc_;
		uint8_t pt_;
		RTPSessionPrt rtpsession_;
	};

	struct BufferNode
	{
		BufferNode()
		{
			data_ = new uint8_t[1024 * 1024];
		}

		BufferNode(BufferNode && other)
			:data_(other.data_), length_(other.length_)
		{
			other.data_ = nullptr;
			other.length_ = 0;
		}
		BufferNode & operator=(BufferNode && other)
		{
			//destip_ = std::move(other.destip_);
			//destport_ = other.destport_;
			//dests_ = std::move(other.dests_);
			length_ = other.length_;
			data_ = other.data_;

			other.data_ = nullptr;
			other.length_ = 0;
			return *this;
		}

		BufferNode(const BufferNode & other) = delete;
		BufferNode & operator=(const BufferNode & other) = delete;

		~BufferNode()
		{
			if (data_)
			{
				//std::cout << "free buffer node\n";
				delete[] data_;
			}
		}
		unsigned char * data_;
		uint32_t length_;
	};

	void push(char * data, uint32_t len);
	std::shared_ptr<BufferNode> getFreeNode();
	void push(std::shared_ptr<BufferNode> node);

	uint32_t destip_;
	uint32_t destport_;
	uint32_t ssrc_;
	uint8_t pt_;

	void addDestination(std::string callid ,uint32_t destip,uint32_t destport,uint32_t ssrc,
		                uint8_t pt/*, jrtplib::RTPTransmitter* transmitter*/);

	void removeDestination(const std::string &callid) ;

	void processData();

	void getNodeInfo(int &b, int &f);
public:
	static void checkerror(int rtperr);

private:
	threadsafe_queue<BufferNode> freeChain_;
	threadsafe_queue<BufferNode> busyChain_;
	std::map<std::string, Destination> dests_;
	
	std::condition_variable cond_;
	std::mutex freemutex_;
	std::mutex busymutex_;
	std::mutex destsmutex_;
	jrtplib::RTPUDPv4TransmissionParams * transParams_;
	jrtplib::RTPTransmitter* transmitter_;
#ifdef _DEBUG
	std::ofstream file;
#endif // _DEBUG

};


