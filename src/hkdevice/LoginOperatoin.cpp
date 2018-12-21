#include "LoginOperatoin.h"
#include <cassert>
#include <Windows.h>
#include <BaseTsd.h>
#include <WinNT.h>
#include "HCNetSDK.h"
#include "glog\logging.h"
#include "SdkUsers.h"

//LoginOperatoin::LoginOperatoin(std::shared_ptr<Device> device, std::shared_ptr<DeviceProcesser> processer) :
//	AbstractOperation(device, processer)
//{
//}
//extern "C"  DeviceProcesser* GetProcesser()
//{
//	//std::shared_ptr<DeviceProcesser> processor = std::make_shared<HikDeviceProcesser>();
//	//HikDeviceProcesser processor;
//	//std::thread processthread(&DeviceProcesser::run, processor);
//	LOG(INFO) << "Create HikDeviceProcesser";
//	return new HikDeviceProcesser;
//	//return processor;
//}

LoginOperatoin::LoginOperatoin(std::shared_ptr<Device> device):
	AbstractOperation(device), needInitSdk(true)
{
}

LoginOperatoin::~LoginOperatoin()
{
}

void LoginOperatoin::Process()
{
	//assert(processer_);
	assert(device_);
	//device_->Login();
	{
		//调用海康sdk登录
		//LOG_TRACE(g_nLogID, "Enter");
		//BaseModel Model;
		//if (m_pHikDevice->GetModel(request.stDeviceID, Model))//检测当前设备是否已经连接成功
		//{
		//	request.outputparam.pthis = m_pHikDevice->m_pRecvResponseThis;
		//	request.outputparam.nErrorID = G_ERROR_ALREADY_LOGIN;
		//	m_pHikDevice->m_pRecvResponse(request);
		//	LOG_WARN(g_nLogID, "该设备没有登录,SIPID:" << request.stDeviceID.a << request.stDeviceID.b);
		//	LOG_TRACE(g_nLogID, "Exit");
		//	return false;
		//}
		if (needInitSdk)
		{
			LOG(INFO) << "Initialize Hik SDK";
			NET_DVR_Init();
			needInitSdk = false;
		}
		NET_DVR_USER_LOGIN_INFO struLoginInfo = { 0 };
		struLoginInfo.bUseAsynLogin = false;
		memcpy(struLoginInfo.sDeviceAddress, device_->userIp_.c_str(), NET_DVR_DEV_ADDRESS_MAX_LEN);
		memcpy(struLoginInfo.sUserName, device_->loginName_.c_str(), NAME_LEN);
		memcpy(struLoginInfo.sPassword, device_->loginPassword_.c_str(), PASSWD_LEN);
		struLoginInfo.wPort = atoi(device_->userPort_.c_str());

		NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = { 0 };

		LONG lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
		if (lUserID < 0)
		{
			LOG(ERROR) << "注册设备失败:\r\n"
				<< "设备IP:[" << device_->userIp_ << "]\r\n"
				<< "设备端口:[" << device_->userPort_ << "]\r\n"
				<< "用户名:[" << device_->userName_ << "]\r\n"
				<< "密码:[" << device_->loginPassword_ << "]\r\n"
				<< "错误码: " << NET_DVR_GetLastError();
			//<< "SIPID:[" << request.stDeviceID.a << request.stDeviceID.b << "]";
			//m_pHikDevice->InsertModel(request.stDeviceID, sDeviceAddress, wPort, sUserName, sPassword, lUserID);
			//request.outputparam.pthis = m_pHikDevice->m_pRecvResponseThis;
			//request.outputparam.nErrorID = NET_DVR_GetLastError();
			//m_pHikDevice->m_pRecvResponse(request);
			return;
		}
		NET_DVR_DEVICECFG_V40 devConfig;
		memset(&devConfig, 0, sizeof(LPNET_DVR_DEVICECFG_V40));
		DWORD  dwBytesReturned = 0;
		BOOL ret = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_DEVICECFG_V40, 0, &devConfig, sizeof(devConfig), &dwBytesReturned);
		LOG(INFO) << "device type : " << devConfig.byDevTypeName;

		std::shared_ptr<SdkUser> user = std::make_shared<SdkUser>();
		user->SetUserId(lUserID);
		user->SetChannelId(0);
		user->Online(true);
		SdkUserManager::GetInstance()->Insert(device_->userId_, user);

		if (ret)
		{
	/*		std::shared_ptr<SdkUser> user = std::make_shared<SdkUser>();
			user->SetUserId(lUserID);
			user->GetChannelId(0);
			SdkUserManager::GetInstance()->Insert(device_->userId, user);*/
		}
		else
		{
			//m_pHikDevice->InsertModel(request.stDeviceID, sDeviceAddress, wPort, sUserName, sPassword, lUserID);
		}
		return;
	}
}
