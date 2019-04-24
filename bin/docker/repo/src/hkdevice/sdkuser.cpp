#include "SdkUsers.h"

SdkUser::SdkUser():
	sdkUserid_(-1), sdkChannelId_(-1), isOnline_(false), sdkPlayHandle_(-1),isPlaying_(false)//,playCount_(0)
{
}

SdkUser::~SdkUser()
{
}

void SdkUser::SetCallBack(std::function<void(char*, uint32_t)> cb)
{
	cb_ = cb;
}

bool SdkUser::operator==(const SdkUser & other)
{
	if ( (sdkUserid_ == other.sdkUserid_) && (sdkChannelId_ == other.sdkChannelId_))
	{
		return true;
	}
	return false;
}

bool SdkUser::operator!=(const SdkUser &other)
{
	if ((sdkUserid_ != other.sdkUserid_) || (sdkChannelId_ != other.sdkChannelId_))
	{
		return true;
	}
	return false;
}

SdkUserManager* SdkUserManager::instance_ = nullptr;
SdkUserManager * SdkUserManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new SdkUserManager;
	}
	return instance_;
}

void SdkUserManager::Insert(std::string id, std::shared_ptr<SdkUser> sdkuser)
{
	//users_.insert(std::make_pair(id, sdkuser));
	users_[id] = sdkuser;
}

void SdkUserManager::Update(std::string id, std::shared_ptr<SdkUser> user)
{
	std::map<std::string, std::shared_ptr<SdkUser>>::iterator it = users_.find(id);
	if (it != users_.end())
	{
		if (*(it->second) != (*user))
		{
			it->second = user;
		}
	}
}

std::shared_ptr<SdkUser> SdkUserManager::GetSdkUser(std::string id)
{
	std::map<std::string, std::shared_ptr<SdkUser>>::iterator it = users_.find(id);
	if (it != users_.end())
	{
		return it->second;
	}
	return nullptr;
}

SdkUserManager::SdkUserManager()
{
}

SdkUserManager::~SdkUserManager()
{
}