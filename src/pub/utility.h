#pragma once
#include<functional>
//#include "Gateway.h"
#include "Catalog.h"

namespace Media
{
	struct RealStreamReqParam;
	struct RealStreamRespParam;
};

namespace dt
{

	struct OpenRealStream
	{
		OpenRealStream(const Media::RealStreamReqParam& param, std::shared_ptr<CCatalog> ctg ,
			           std::function<void(const::Media::RealStreamRespParam&)> cb,
			           std::function<void(::std::exception_ptr)> excb);
		std::string id;
		std::string callid;
		std::string destip;
		int destport;
		int pt;
		int ssrc;

		std::function<void(char *, uint32_t)> datacb_;
		std::function<void(const::Media::RealStreamRespParam&)> cb_;  //normal callback
		std::function<void(::std::exception_ptr)> excb_;  //exception callback 

		std::string ip;
		int port;
		std::string name;
		std::string pwd;
		bool isAsyn;

	};
};

