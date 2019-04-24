#ifdef WINDOWS
#include <winsock2.h>
#include<windows.h>
#endif
#include "streamI.h"
//#define WIN32_LEAN_AND_MEAN
#include "mediastream.h"
#include "message.h"
#include "glog/logging.h"
#include "icecallback.h"
#include "media.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>


StreamI::StreamI()
	//:sm_(new StreamManager())
{
	
}

StreamI::~StreamI()
{
}

void StreamI::openRealStreamAsync(::Media::RealStreamReqParam param, ::std::function<void(bool, const::Media::RealStreamRespParam&)> cb, ::std::function<void(::std::exception_ptr)> ecb, const::Ice::Current &)
{
	LOG(INFO) << "call id: " << param.callid;
	dt::OpenRealStreamParam p;
	p.id = param.id;
	p.callid = param.callid;
	p.ip_ = param.ip;
	p.port_ = param.port;
	p.userName_ = param.userName;
	p.password_ = param.password;
	p.destip = param.destip;
	p.destport = param.destport;
	p.ssrc = param.ssrc;
	p.pt = param.pt;

	if(param.callid.empty())
	{
		boost::uuids::random_generator generator;
		boost::uuids::uuid uuid = generator();
		p.callid = boost::uuids::to_string(uuid);
		LOG(INFO) << "call id: " << param.callid;
	}

	//std::shared_ptr<CallBack> callback = std::make_shared<IceCallBack<std::function<void(bool, const::Media::RealStreamRespParam&)>>>(cb);
    std::shared_ptr<CallBack> callback = std::make_shared<IceCallBack>(cb);
	Meida::getInstance()->openStream(p, callback);
	//StreamManager::getInstance()->addStream(p, cb);
}

void StreamI::closeStreamAsync(::std::string id, ::std::string callid, ::std::function<void()> cb, ::std::function<void(::std::exception_ptr)> excb, const::Ice::Current &)
{
	std::shared_ptr<CallBack> callback = std::make_shared<IceCloseCallBack>(cb);
	Meida::getInstance()->closeStream(id, callid, callback);

	//StreamManager::getInstance()->closeStream(id, callid);
	//cb();
}

void StreamI::getStreamStatic(::std::string id, ::Media::StreamStatic & stat, const::Ice::Current &)
{
	//StreamManager::getInstance()->getStreamStatic(id, stat);
}

//void StreamI::openRealStream_async(const Media::AMD_Stream_openRealStreamPtr &cb, const Media::RealStream & ctg, const Ice::Current &)
//{
//	sm_->addStream(ctg.id,ctg.name, ctg.pwd, ctg.ip, ctg.port, ctg.destip, ctg.destport, ctg.ssrc, cb);
//	//Media::StreamInfo stm;
//	//stm.ip = "111111";
//	//cb->ice_response(stm);
//}

