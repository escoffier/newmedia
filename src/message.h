#pragma once

#include<string>

namespace dt
{
	// struct DeviceStatus
	// {
	// 	std::string id;
	// 	int status;
	// };

	struct OpenRealStreamParam
	{
		std::string id;
		std::string callid;
		std::string ip_;
		int port_;
		std::string userName_;
		std::string password_;
		std::string destip;
		int destport;
		int pt;
		int ssrc;
	};


	struct OpenRealStreamResp
	{
		std::string id;
		std::string callid;
		std::string sourceip;
		int sourceport;
		bool succed;
	};
};