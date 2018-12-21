#pragma once

#include "DeviceProcesser.h"
#include "SdkUsers.h"
#include <mutex>
#include <unordered_map>

class HikCamaraController : public CamaraController
{
public:
	using dataCallBack = std::function<void(char *, uint32_t)>;

	HikCamaraController();
	~HikCamaraController();

	virtual bool Login(std::string id, std::string ip, int port, std::string user, std::string pwd);
	
	virtual bool isLogin() const;
	virtual void ptzControl(std::string id, std::string cmd);
	virtual void getDeviceStatusAsync(std::string id, std::function<void(int)> cb);
	virtual void getDeviceStatus(std::string id, dt::DeviceStatus &st);
	//virtual bool openRealStream(const dt::OpenRealStream& param);
	virtual bool openRealStream(std::string id, std::string ip, int port, std::string user,
		std::string pwd, std::function<void(char *, uint32_t)> datacb_);
	virtual bool closeRealStream(const std::string& id);
	//virtual void setDataCallback(dataCallBack cb);

	void processData(long hd, char *, uint32_t);
	//std::string getIdbyPlayHandle(long hd);

private:
	bool logIn(std::string id, std::string ip, int port, std::string user, std::string pwd);
	bool Login(std::shared_ptr<SdkUser> user);
private:
	//WorkorQueue queue_;
	bool login_;
	bool isOpen_;
	std::map<std::string, std::shared_ptr<SdkUser> > sdkusers_;
	//std::unordered_map<long, dataCallBack> cbMap_;
	std::unordered_map<long, dataCallBack*> cbMap_;
	std::mutex streamMutex_;
	std::function<void(char *, uint32_t)> cb_;
};


