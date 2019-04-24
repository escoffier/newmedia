#include "RTPSender.h"
#include "glog/logging.h"
//#include "PSBuffer.h"

static void checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		LOG(ERROR) << "jrtplib error: " << jrtplib::RTPGetErrorString(rtperr);
	}
}

#if 0
void RTPSender::sendRTPPacket()
{
	while (true)
	{
		std::lock_guard<std::mutex> lk(mutex_);
		
		if (bufferMap_.empty())
		{
			std::this_thread::yield();
		}

		auto it = bufferMap_.begin();
		while (it != bufferMap_.end())
		{
			it->second->processData();
			it++;	
		}
	}
}

#define RTP_MAX_PACKET_LEN 1450

RTPSender::RTPSender():
	mutex_()//, buffer_(std::make_shared<PSBuffer>())
{
#ifdef RTP_SOCKETTYPE_WINSOCK
	WSADATA dat;
	WSAStartup(MAKEWORD(2, 2), &dat);
#endif // RTP_SOCKETTYPE_WINSOCK
	transParams_ = new jrtplib::RTPUDPv4TransmissionParams;
	transParams_->SetRTPSendBuffer(65535); // default: 32768
	transParams_->SetPortbase(16000);
	uint32_t localip;
	localip = inet_addr("192.168.21.221");
	localip = ntohl(localip);

	transParams_->SetBindIP(localip);
	transmitter_ = new jrtplib::RTPUDPv4Transmitter(nullptr);
	int ret = transmitter_->Init(true);
	checkerror(ret);
	ret = transmitter_->Create(1400, transParams_);
	checkerror(ret);

	senderThread_ = std::thread(&RTPSender::sendRTPPacket, this);
}

void RTPSender::addBuffer(std::string id, std::shared_ptr<PSBuffer> buf)
{
	std::lock_guard<std::mutex> lk(mutex_);
	bufferMap_.emplace(std::make_pair(id, buf));
}

void RTPSender::removeBuffer(std::string id)
{
	std::lock_guard<std::mutex> lk(mutex_);
	bufferMap_.erase(id);
}

#endif


#define RTP_MAX_PACKET_LEN 1450
jrtplib::RTPTransmitter * RTPSender::transmitter_ = nullptr;

bool RTPSender::createRTPTransmitter()
{
#ifdef RTP_SOCKETTYPE_WINSOCK
	WSADATA dat;
	WSAStartup(MAKEWORD(2, 2), &dat);
#endif // RTP_SOCKETTYPE_WINSOCK
    jrtplib::RTPUDPv4TransmissionParams *transParams_;
	transParams_ = new jrtplib::RTPUDPv4TransmissionParams;
	transParams_->SetRTPSendBuffer(65535); // default: 32768
	transParams_->SetPortbase(18000);
	uint32_t localip;
	localip = inet_addr("192.168.21.197");
	localip = ntohl(localip);

	transParams_->SetBindIP(localip);
	transmitter_ = new jrtplib::RTPUDPv4Transmitter(nullptr);
	int ret = transmitter_->Init(true);
	checkerror(ret);
	ret = transmitter_->Create(1400, transParams_);
	checkerror(ret);
	return true;
}

RTPSender::RTPSender(const std::string &destIp, int destPort) : PaketSender(destIp, destPort), rtpsession_(new jrtplib::RTPSession)
{
}

bool RTPSender::init()
{
// #ifdef RTP_SOCKETTYPE_WINSOCK
// 	WSADATA dat;
// 	WSAStartup(MAKEWORD(2, 2), &dat);
// #endif // RTP_SOCKETTYPE_WINSOCK
// 	transParams_ = new jrtplib::RTPUDPv4TransmissionParams;
// 	transParams_->SetRTPSendBuffer(65535); // default: 32768
// 	transParams_->SetPortbase(18000);
// 	uint32_t localip;
// 	localip = inet_addr("192.168.21.197");
// 	localip = ntohl(localip);

// 	transParams_->SetBindIP(localip);
// 	transmitter_ = new jrtplib::RTPUDPv4Transmitter(nullptr);
// 	int ret = transmitter_->Init(true);
// 	checkerror(ret);
// 	ret = transmitter_->Create(1400, transParams_);
// 	checkerror(ret);

	auto ssrc = paramsmap_.find("ssrc");
	if (ssrc == paramsmap_.end())
	{
		// delete transmitter_;
		// delete transParams_;
		return false;
	}

	int nssrc = atoi((ssrc->first).c_str());
	createRTPSession(nssrc, transmitter_);
	return true;
}

void RTPSender::send(uint8_t *data, size_t len)
{
	while (len >= 1400)
	{
		rtpsession_->SendPacket(data, 1400, 96, false, 0);
		//fwrite(data, 1, 1400, streamfile);
		//checkerror(rtpret);
		data += 1400;
		len -= 1400;
	}

	if (len > 0)
	{
		rtpsession_->SendPacket(data, len, 96, false, 3600);
		//fwrite(data, 1, len, streamfile);
	}
}

int RTPSender::createRTPSession(uint32_t ssrc, jrtplib::RTPTransmitter *transmitter)
{
	jrtplib::RTPSessionParams sessionParams;
	sessionParams.SetOwnTimestampUnit(1.0 / 90000.0); //RTP_TIMESTAMP_UNIT);
	sessionParams.SetAcceptOwnPackets(true);
	sessionParams.SetMaximumPacketSize(RTP_MAX_PACKET_LEN);
	sessionParams.SetPredefinedSSRC(ssrc);

	//RTPSessionPrt rtpss_(new jrtplib::RTPSession);
	int rtpret = rtpsession_->Create(sessionParams, transmitter);
	::checkerror(rtpret);

	rtpret = rtpsession_->SetDefaultPayloadType(96);
	::checkerror(rtpret);
	rtpret = rtpsession_->SetDefaultMark(false);
	::checkerror(rtpret);
	rtpret = rtpsession_->SetDefaultTimestampIncrement(3600);
	::checkerror(rtpret);

	auto ip = inet_addr(destip_.c_str());
	ip = ntohl(ip);
	

	jrtplib::RTPIPv4Address addr(ip, destport_);
	LOG(ERROR) << "rtp dest port: " << destport_;
	rtpret = rtpsession_->AddDestination(addr);
	::checkerror(rtpret);

	return rtpret;
}