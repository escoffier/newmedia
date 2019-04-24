#include "streammanager.h"
#include "threadpool.h"
#include "mediastream.h"
#include "HCNetSDK.h"
#include "glog/logging.h"
#include "zmd5.h"
#include "hikcontroller.h"

StreamManager *StreamManager::instance_ = nullptr;
//template<>  ThreadPool<PSBuffer> * ThreadPool<PSBuffer>::instance_ = nullptr;

StreamManager *StreamManager::getInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new StreamManager;
	}

	return instance_;
}

StreamManager::StreamManager() : streams_(), msgthread_(), locker_(), /*rtpSender_(new RTPSender),*/ plugins_(new SDKPlugins),
								 avThreadPool_(std::make_shared<ThreadPool>(4))
{
	NET_DVR_Init();
	auto hikcontrollser = std::make_shared<HikCamaraController>();
	controllers_.emplace("hkdevice", hikcontrollser);

	RTPSender::createRTPTransmitter();
};

std::shared_ptr<MediaStream> StreamManager::getStream(const std::string &id)
{
	auto search = streams_.find(id);
	if (search != streams_.end())
	{
		return search->second;
	}
	return nullptr;
}

// void StreamManager::addStream(Media::RealStreamReqParam &param,
// 							  const std::function<void(bool, const ::Media::RealStreamRespParam &)> iceCB,
// 							  std::function<void(::std::exception_ptr)> ecb)
// {
// 	//std::lock_guard<std::mutex> lockGuard(locker_);

// 	//auto search = streams_.find(id);
// 	//if (search == streams_.end())
// 	//{
// 	//	std::shared_ptr<MediaStream> ms = std::make_shared<RealSteam>(id, name, pwd, ip, port, destIp, destPort, iceCB, ecb);

// 	//    ms->addsubStream(destIp, destPort, ssrc, transmitter_);
// 	//	streams_.emplace(id, ms);
// 	//
// 	//	msgthread_.submit([=]() {
// 	//		openStream(id, callid);
// 	//	});
// 	//}
// 	//else
// 	//{
// 	//	//search->second->addDestHost(destIp, destPort);
// 	//	search->second->addsubStream(destIp, destPort, ssrc, transmitter_);
// 	//}
// }

void StreamManager::addStream(const dt::OpenRealStreamParam &param, std::shared_ptr<CallBack> cb)
{
	std::lock_guard<std::mutex> lockGuard(locker_);

	std::shared_ptr<CamaraController> controller;
	dt::OpenRealStreamResp resp;
	resp.id = param.id;

	auto search = streams_.find(param.id);
	if (search == streams_.end())
	{
		controller = getController("hkdevice");
		if (controller == nullptr)
		{
			LOG(ERROR) << "cann't found hkdevice";
			resp.succed = false;
			(*cb)(resp);
			return;
		}

		std::shared_ptr<MediaStream> ms = std::make_shared<RealSteam>(param, cb, controller, avThreadPool_);
		resp.id = param.id;

		ms->addsubStream(param.callid, param.destip, param.destport,
						 param.ssrc, param.pt, resp);
		if (!resp.succed)
		{
			LOG(ERROR) << "Add sub-stream failed by id: " << param.id << std::endl;
			resp.succed = false;
			(*cb)(resp);
			return;
		}

		streams_.emplace(param.id, ms);

		msgthread_.submit([=]() mutable {
			//dt::OpenRealStreamResp resp;
			if (!ms->openStream(param.callid))
			{
				LOG(ERROR) << "Cann't find stream: " << param.id << std::endl;
				ms->removesubStream(param.callid);
				resp.succed = false;
				(*cb)(resp);
				return;
			}
			else
			{
				resp.succed = true;
				resp.callid = param.callid;
				LOG(ERROR) << "Open stream: " << param.callid << std::endl;
				(*cb)(resp);
			}
		});
	}
	else //stream from id camera already existed
	{
		search->second->addsubStream(param.callid, param.destip, param.destport, param.ssrc, param.pt, resp);
		if (!resp.succed)
		{
			resp.succed = false;
			(*cb)(resp);
			return;
		}
		else
		{
			resp.succed = true;
			resp.callid = param.callid;
			(*cb)(resp);
		}
	}
}

void StreamManager::closeStream(const std::string &id, const std::string &callid, std::shared_ptr<CallBack> cb)
{
	std::lock_guard<std::mutex> lockGuard(locker_);
	auto search = streams_.find(id);
	if (search == streams_.end())
	{
		LOG(ERROR) << "Cann't find stream: " << id << std::endl;
		(*cb)();
	}
	else
	{
		auto stream = search->second;
		stream->removesubStream(callid);
		auto buffer = stream->getBuffer();
		//search->second->removesubStream(callid);

		//auto buf = avThreadPool_->getBuffer(id);
		if (buffer != nullptr)
		{
			buffer->removeSender(callid);
		}

		if (stream->needClose())
		{
			stream->closeStream();
			avThreadPool_->removeBuffer(id);
			streams_.erase(search);
		}
	}
	(*cb)();
}

void StreamManager::getStreamStatic(std::string id, ::Media::StreamStatic &stat)
{
	// std::lock_guard<std::mutex> lockGuard(locker_);
	// auto search = streams_.find(id);
	// if (search == streams_.end())
	// {
	// 	LOG(ERROR) << "(getStreamStatic)Cann't find stream: " << id << std::endl;
	// 	return;
	// }
	// else
	// {
	// 	search->second->getNodeInfo(stat.busynode, stat.freenode);
	// }
	// return;
}

typedef CamaraController *(*CREATE_PTR)();

std::shared_ptr<CamaraController> StreamManager::loadPlugin(const std::string &name)
{
	if (!plugins_->Load("./", name.c_str()))
	{
		LOG(ERROR) << "Load " << name << ".dll failed!";
		return std::shared_ptr<CamaraController>();
	}

	CREATE_PTR func = (CREATE_PTR)plugins_->GetFuncAddr("GetController");
	if (func == nullptr)
	{
		LOG(ERROR) << "GetFuncAddr failed!";
		return std::shared_ptr<CamaraController>();
	}

	std::shared_ptr<CamaraController> controller = std::shared_ptr<CamaraController>(func());
	if (controller == nullptr)
	{
		LOG(ERROR) << "CamaraController is null!";
		return std::shared_ptr<CamaraController>();
	}

	controllers_.emplace(std::make_pair(name, controller));
	LOG(INFO) << "load plugin : " << name << ".dll";

	return controller;
}

std::shared_ptr<CamaraController> StreamManager::getController(const std::string &name)
{
	auto search = controllers_.find(name);
	std::shared_ptr<CamaraController> controller;

	if (search == controllers_.end())
	{
		controller = loadPlugin("hkdevice");
		if (!controller)
		{
			LOG(ERROR) << "load plugin failed";
			//Gateway::DeviceControlException ex;
			//ex.reason = "load plugin failed";
			//excb(make_exception_ptr(ex));
			return std::shared_ptr<CamaraController>();
		}
	}
	else
	{
		controller = search->second;
	}

	return controller;
}

std::string StreamManager::buildCallId(const std::string &ip, int port)
{
	unsigned char buff[128] = {0};
	md5_state_t mdctx;
	md5_byte_t md_value[16];

	snprintf((char *)buff, 127, "%s%d", ip.c_str(), port);
	md5_init(&mdctx);
	md5_append(&mdctx, (const unsigned char *)(buff), strlen((const char *)buff));
	md5_finish(&mdctx, md_value);

	char md5sum[33];
	int i;
	int h, l;
	for (i = 0; i < 16; ++i)
	{
		h = md_value[i] & 0xf0;
		h >>= 4;
		l = md_value[i] & 0x0f;
		md5sum[i * 2] = (char)((h >= 0x0 && h <= 0x9) ? (h + 0x30) : (h + 0x57));
		md5sum[i * 2 + 1] = (char)((l >= 0x0 && l <= 0x9) ? (l + 0x30) : (l + 0x57));
	}
	md5sum[32] = '\0';

	std::string md5str(md5sum);
	return md5str;
}