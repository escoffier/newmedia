#ifdef WINDOWS
#include <winsock2.h>
#include <windows.h>
#endif
#include "mediastream.h"

#include <functional>
#include "threadpool.h"
#define WIN32_LEAN_AND_MEAN
#include "HCNetSDK.h"
#include "glog/logging.h"
#include "RTPSender.h"
//#include "callback.h"

FILE *streamfile = fopen("stream106", "wb");

void checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout << "ERROR: " << jrtplib::RTPGetErrorString(rtperr) << std::endl;
		//exit(-1);
	}
}

MediaStream::MediaStream(const dt::OpenRealStreamParam &req, std::shared_ptr<CallBack> cb,
						 std::shared_ptr<CamaraController> con, std::shared_ptr<ThreadPool> tp) : id_(req.id), userName_(req.userName_), password_(req.password_), ip_(req.ip_), port_(req.port_), destIp_(req.destip),
																								  destPort_(req.destport), avThreadPool_(tp), controller_(con)
{
}

void MediaStream::addDestHost(const std::string &destIp, int destPort)
{
}

void MediaStream::addsubStream(const std::string &callid, const std::string &destIp, int destPort,
							   int ssrc, int pt, dt::OpenRealStreamResp &resp)
{
	std::lock_guard<std::mutex> lockGuard(mutex_);
	auto search = subStreams_.find(callid);
	if (search == subStreams_.end())
	{
		LOG(INFO) << "add substream : " << destIp << " - " << destPort << std::endl;
		//Create buffer
		buffer_ = std::make_shared<Buffer>();
		//create rtp sender
		std::shared_ptr<PaketSender> sender = std::make_shared<RTPSender>(destIp, destPort);
		sender->setParam("ssrc", "1111");
		if (!sender->init())
		{
			LOG(ERROR) << "sender initial failed";
			resp.succed = false;
			return;
		}

		buffer_->addSender(callid, sender);

		avThreadPool_->attachBuffer(id_, buffer_);

		//buffer_->addDestination(callid, ss.destipn_, destPort, ssrc, pt/*, transmitter*/);
		resp.callid = callid;
		resp.succed = true;

		subStream ss(destIp, destPort, "192.168.21.197", 1999, ssrc /*, transmitter_*/);

		subStreams_.emplace(callid, std::move(ss));
	}
	else
	{
		LOG(ERROR) << "Sub-stream  already existed";
		resp.succed = false;
	}
	return;
}

void MediaStream::removesubStream(const std::string &callid)
{
	{
		std::lock_guard<std::mutex> lockGuard(mutex_);
		subStreams_.erase(callid);
	}
	//buffer_->removeDestination(callid);
	buffer_->removeSender(callid);
}

size_t MediaStream::subStreamCount() const
{
	//std::lock_guard<std::mutex> lockGuard(mutex_);
	return subStreams_.size();
}

bool MediaStream::needClose()
{
	std::lock_guard<std::mutex> lockGuard(mutex_);
	return subStreams_.empty();
}

//this function is called in other thread
void MediaStream::processData(char *data, uint32_t len)
{
	//LOG(INFO) << "processing data";
	std::shared_ptr<AVPacket> pkt = std::make_shared<AVPacket>(len);
	buffer_->pushPacket(pkt);
	// std::shared_ptr<PSBuffer::BufferNode> node = buffer_->getFreeNode();
	// if (!node)
	// {
	// 	LOG(WARNING) << "discard data";
	// 	return;
	// }
}

RealSteam::RealSteam(const dt::OpenRealStreamParam &req, std::shared_ptr<CallBack> cb,
					 std::shared_ptr<CamaraController> con, std::shared_ptr<ThreadPool> tp) : MediaStream(req, cb, con, tp)
{
}

RealSteam::~RealSteam()
{
	LOG(ERROR) << " destruct RealSteam";
	if (!needClose())
		closeStream();
}

bool RealSteam::openStream(const std::string &callid)
{
	LOG(ERROR) << " RealSteam openStream";
	if (!controller_)
	{
		LOG(ERROR) << "openStream failed";
		return false;
	}
	LOG(INFO) << "id_" << id_ << "ip_" << ip_
			  << "port_" << port_
			  << "userName_" << userName_
			  << "password_" << password_;
	if (controller_->openRealStream(id_, ip_, port_, userName_, password_,
									[=](char *data, uint32_t len) { processData(data, len); }))
	{
		Media::RealStreamRespParam resp;
		//respinfo.callid = callid;
		//uint32_t ip = transParams.GetBindIP();
		//char strIp[16] = {0};
		//inet_ntop(AF_INET, (void*)&ip, strIp, 15);
		std::string strIp("192.168.254.233");
		resp.sourceip = strIp;
		resp.callid = callid;
		resp.id = id_;
		return true;
	}
	else
	{
		//ecb_();
		return false;
	}
}

void RealSteam::closeStream(const std::string &callid)
{
	//std::lock_guard<std::mutex> lockGuard(mutex_);
	//auto search = subStreams_.find(callid);
	//if (search != subStreams_.end())
	//{
	//	size_t n =  subStreams_.count(callid);
	//	if (n == 0)
	//	{
	//		if (controller_->closeRealStream(search->second.id_))
	//		{
	//			LOG(ERROR) << "Hik sdk close stream failed, error code: " << NET_DVR_GetLastError();
	//		}
	//		else
	//		{
	//			LOG(INFO) << "Close stream: " << callid;
	//		}
	//
	//	}
	//	subStreams_.erase(search);
	//}

	return;
}

void RealSteam::closeStream()
{
	if (!controller_->closeRealStream(id_))
	{
		LOG(ERROR) << "Hik sdk close stream failed, error code: " << NET_DVR_GetLastError();
	}
	else
	{
		LOG(INFO) << "Close stream: " << id_;
	}
}

// VodSteam::VodSteam(const std::string &id, const std::string &name, const std::string &pwd,
// 				   const std::string &ip, int port, const std::string &destIp, int destPort,
// 				   std::function<void(const ::Media::RealStreamRespParam &)> iceCB, std::function<void(::std::exception_ptr)> ecb,
// 				   const std::string &startTime, const std::string &endTime, std::shared_ptr<CamaraController> con) : MediaStream(id, name, pwd, ip, port, destIp, destPort, iceCB, ecb, con), startTime_(startTime), endTime_(endTime)
VodSteam::VodSteam(const dt::OpenRealStreamParam &req, std::shared_ptr<CallBack> cb,
				   std::shared_ptr<CamaraController> con, std::shared_ptr<ThreadPool> tp) : MediaStream(req, cb, con, tp)
{
}

VodSteam::~VodSteam()
{
}

bool VodSteam::openStream(const std::string &callid)
{
	return false;
}

void VodSteam::closeStream(const std::string &callid)
{
}

void VodSteam::closeStream()
{
}
