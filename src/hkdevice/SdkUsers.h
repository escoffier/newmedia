#pragma once
#include <map>
#include<memory>
#include <atomic>
#include <functional>
#ifdef WINDOWS
#include <Windows.h>
#endif // WINDOWS

//#include "glog\logging.h"

class SdkUser
{
public:
	//typedef std::function<void(std::string id, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize)> ProcFunc;
	SdkUser();
	~SdkUser();
	long GetUserId() const { return sdkUserid_; }
	long GetChannelId() const { return sdkChannelId_; }
	long GetPlayHandle() const { return sdkPlayHandle_; }
	//ProcFunc DataProcFunc() { return dataProc_; }
	std::string GetDeviceId() { return deviceid_; }

	void SetUserId(long id)  { sdkUserid_ = id; }
	void SetChannelId(long id) { sdkChannelId_ = id; }
	void SetPlayHandle(long handle) { sdkPlayHandle_ = handle; }
	//void SetDataProc(ProcFunc dataProc) { dataProc_ = dataProc; }
	void SetDeviceId(std::string id) { deviceid_ = id; }
	void SetCallBack(std::function<void(char *, uint32_t)> cb);
	bool IsOnline() {
		return isOnline_;}
	bool IsPlaying() { return isPlaying_.load(); }

	void Online(bool online) { isOnline_ = online; }
	void Playing(bool playing) { isPlaying_ = playing; }
	//void IncrePlayCount() {
	//	playCount_.fetch_add(1);
	//	LOG(INFO)<<"playCount_: "<<playCount_.load(); 
	//	isPlaying_.store( true);
	//}
	//void DecrePlayCount() 
	//{ 
	//	int cunt = playCount_.fetch_sub(1);
	//	if (cunt == 0)
	//	{
	//		isPlaying_.store(false);
	//	}
	//	else if (cunt < 0)
	//	{
	//		playCount_.store(0);
	//		isPlaying_.store( false);
	//	}
	//}

	bool operator==(const SdkUser &);
	bool operator!=(const SdkUser &);
public:
	long sdkUserid_;
	long sdkChannelId_;
	bool isOnline_;
	long sdkPlayHandle_;
	//bool isPlaying_;
	std::atomic_bool isPlaying_;
	//std::atomic_int playCount_;
	std::string deviceid_;
	std::string ip_;
	int port_;
	std::string userName_;
	std::string password_;
	std::function<void(char *, uint32_t)> cb_;
};

class SdkUserManager
{
public:
	static SdkUserManager* GetInstance();
	void Insert(std::string id, std::shared_ptr<SdkUser>);
	void Update(std::string id, std::shared_ptr<SdkUser>);
	std::shared_ptr<SdkUser> GetSdkUser(std::string id);
private:
	SdkUserManager();
	~SdkUserManager();
	SdkUserManager(const SdkUserManager &) = delete;
	SdkUserManager& operator=(const SdkUserManager &) = delete;
private:
	static SdkUserManager* instance_;
	std::map<std::string, std::shared_ptr<SdkUser>> users_;
};




