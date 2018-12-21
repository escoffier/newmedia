#pragma once
//#include <winsock2.h>
#include <string>
#include <memory>
#include <map>
#include <stream.h>
//#include "WorkorQueue.h"
#include "msgthread.h"
// #include "jrtplib3/rtpsession.h"
// #include "jrtplib3/rtpudpv4transmitter.h"
// #include "jrtplib3/rtpipv4address.h"
// #include "jrtplib3/rtpsessionparams.h"
//#include "PSBuffer.h"
#include "buffer.h"
#include "RTPSender.h"
#include "DeviceProcesser.h"
//#include "plugins.h"
#include "message.h"
#include "threadpool.h"
#include "callback.h"

class MediaStream
{
  public:
	//using RTPTransmitterPtr = std::shared_ptr<jrtplib::RTPTransmitter> ;

	// MediaStream(const std::string &id, const std::string &name, const std::string &pwd,
	// 	        const std::string &ip, int port, const std::string &destIp, int destPort,
	// 	        std::function<void(const::Media::RealStreamRespParam&)> iceCB,
	// 	        std::function<void(::std::exception_ptr)> ecb, std::shared_ptr<CamaraController> con);

	MediaStream(const dt::OpenRealStreamParam &req, std::shared_ptr<CallBack> cb, std::shared_ptr<CamaraController> con,
				std::shared_ptr<ThreadPool> tp);

	virtual ~MediaStream()
	{
		std::cout << "MediaStream destroy";
	}

	void addDestHost(const std::string &destIp, int destPort);
	void addsubStream(const std::string &callid, const std::string &destIp, int destPort,
							 int ssrc, int pt, dt::OpenRealStreamResp &resp);

	void removesubStream(const std::string &callid);

	virtual bool openStream(const std::string &callid) = 0;

	virtual void closeStream(const std::string &callid) = 0;

	virtual void closeStream() = 0;

	size_t subStreamCount() const;

	bool needClose();

	std::shared_ptr<Buffer> getBuffer() { return buffer_; };

	void setBuffer(std::shared_ptr<Buffer> buf) { buffer_ = buf; }

	void processData(char *data, uint32_t len);

  protected:
	std::string id_;
	std::string userName_; //camara login name
	std::string password_; //camara login password
	std::string ip_;	   //camcara ip
	int port_;			   //camara port
	std::string destIp_;
	int destPort_;
	std::function<void(const ::Media::RealStreamRespParam &)> iceCB_;
	std::function<void(::std::exception_ptr)> ecb_;
	struct subStream
	{
#define RTP_MAX_PACKET_LEN 1450
		subStream(const std::string &destIp, int destPort, const std::string &ip, int port, int ssrc /*, jrtplib::RTPTransmitter* transmitter*/) : destIp_(destIp), ip_(), destPort_(destPort), port_(port), ssrc_(ssrc), rtpss_(new jrtplib::RTPSession) /*, transmitter_(transmitter)*/,
																																				   respcb_(), excb_()
		{
			destipn_ = inet_addr(destIp_.c_str());
			destipn_ = ntohl(destipn_);
		}
		subStream(const subStream &ss) = delete;
		//subStream(const subStream &ss):
		//	destIp_(ss.destIp_), ip_(ss.ip_), destPort_(ss.destPort_),
		//	port_(ss.port_), ssrc_(ss.ssrc_), rtpss_(std::move(ss.rtpss_))
		//{
		//	destIp_ = ss.destIp_;
		//	destPort_ = ss.destPort_;
		//}

		subStream(subStream &&ss) : destIp_(std::move(ss.destIp_)), ip_(std::move(ss.ip_)), destPort_(ss.destPort_),
									port_(ss.port_), ssrc_(ss.ssrc_), rtpss_(std::move(ss.rtpss_)), respcb_(std::move(ss.respcb_)), excb_(std::move(ss.excb_))
		{
			destipn_ = inet_addr(destIp_.c_str());
			destipn_ = ntohl(destipn_);
		}

		void checkerror(int rtperr)
		{
			if (rtperr < 0)
			{
				std::cout << "ERROR: " << jrtplib::RTPGetErrorString(rtperr) << std::endl;
				//exit(-1);
			}
		}

		~subStream()
		{
			//rtpss_->Destroy();
		}

		std::string id_;
		std::string destIp_;
		std::string ip_;
		int destPort_;
		int port_;
		int ssrc_;
		int pt_;
		std::unique_ptr<jrtplib::RTPSession> rtpss_;
		//jrtplib::RTPTransmitter* transmitter_;
		uint32_t destipn_;
		std::function<void(const ::Media::RealStreamRespParam &)> respcb_;
		std::function<void(::std::exception_ptr)> excb_;
		std::shared_ptr<CamaraController> controller_;
	};

	std::map<std::string, subStream> subStreams_;
	bool isOpen_;
	bool isLogin_;
	std::mutex mutex_;
	std::shared_ptr<Buffer> buffer_;
	std::shared_ptr<CamaraController> controller_;
	std::shared_ptr<ThreadPool> avThreadPool_;
};

class RealSteam : public MediaStream
{
  public:
	RealSteam(const dt::OpenRealStreamParam &req, std::shared_ptr<CallBack> cb,
			  std::shared_ptr<CamaraController> con, std::shared_ptr<ThreadPool> tp);
	~RealSteam();

	virtual bool openStream(const std::string &callid);
	virtual void closeStream(const std::string &callid);
	virtual void closeStream();

  private:
};

class VodSteam : public MediaStream
{
  public:
	// VodSteam(const std::string &id, const std::string &name, const std::string &pwd,
	// 	     const std::string &ip, int port, const std::string &destIp, int destPort,
	// 	     std::function<void(const::Media::RealStreamRespParam&)> iceCB,
	// 	     std::function<void(::std::exception_ptr)> ecb,const std::string &startTime,
	// 	     const std::string &endTime, std::shared_ptr<CamaraController> con);
	VodSteam(const dt::OpenRealStreamParam &req, std::shared_ptr<CallBack> cb,
			 //std::function<void(bool returnValue, const::Media::RealStreamRespParam&)> iceCB,
			 std::shared_ptr<CamaraController> con, std::shared_ptr<ThreadPool> tp);
	~VodSteam();

	virtual bool openStream(const std::string &callid);
	virtual void closeStream(const std::string &callid);
	virtual void closeStream();

  private:
	std::string startTime_;
	std::string endTime_;
};
