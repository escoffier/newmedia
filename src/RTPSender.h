#pragma once
//#include "jrtplib3\rtpsession.h"
#include "jrtplib3/rtpudpv4transmitter.h"
#include "jrtplib3/rtpipv4address.h"
#include "jrtplib3/rtpsessionparams.h"
#include "jrtplib3/rtpsession.h"
#include <memory>
//#include <map>
//#include <mutex>
//#include "PSBuffer.h"
//#include "threadsafequeue.h"
//#include <thread>
#include "packetsender.h"
#if 0
class RTPSender
{
public:
	//using RTPTransmitterPtr = std::shared_ptr<jrtplib::RTPTransmitter>;
	//using RTPSessionPrt = std::unique_ptr<jrtplib::RTPSession>;
	RTPSender();


	~RTPSender()
	{
		if (transmitter_)
		{
			delete transmitter_;
		}
	}

	void sendRTPPacket();
	jrtplib::RTPTransmitter *getRTPTransmitter() { return transmitter_; };

	void addBuffer(std::string, std::shared_ptr<PSBuffer> buf);
	void removeBuffer(std::string id);

private:
	jrtplib::RTPTransmitter* transmitter_;
	jrtplib::RTPUDPv4TransmissionParams * transParams_;
	std::map<std::string, std::shared_ptr<PSBuffer> > bufferMap_;
	//std::list< std::shared_ptr<PSBuffer> > bufferMap_;
	threadsafe_queue<std::shared_ptr<PSBuffer> > bufferQueue_;
	std::mutex mutex_;
	std::thread senderThread_;
};
#endif

class RTPSender : public PaketSender
{
	using RTPSessionPrt = std::unique_ptr<jrtplib::RTPSession>;

  public:
	RTPSender(const std::string & destIp, int destPort);

	bool init() override;
	void send(uint8_t *data, size_t len) override;
    //void setParam(const std::string& key, const std::string &value) override;

	int createRTPSession(uint32_t ssrc, jrtplib::RTPTransmitter *transmitter);
	static bool createRTPTransmitter();

  public:
	static jrtplib::RTPTransmitter *transmitter_;
  private:
	RTPSessionPrt rtpsession_;
	jrtplib::RTPUDPv4TransmissionParams *transParams_;
};