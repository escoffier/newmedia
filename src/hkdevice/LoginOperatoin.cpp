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
		//���ú���sdk��¼
		//LOG_TRACE(g_nLogID, "Enter");
		//BaseModel Model;
		//if (m_pHikDevice->GetModel(request.stDeviceID, Model))//��⵱ǰ�豸�Ƿ��Ѿ����ӳɹ�
		//{
		//	request.outputparam.pthis = m_pHikDevice->m_pRecvResponseThis;
		//	request.outputparam.nErrorID = G_ERROR_ALREADY_LOGIN;
		//	m_pHikDevice->m_pRecvResponse(request);
		//	LOG_WARN(g_nLogID, "���豸û�е�¼,SIPID:" << request.stDeviceID.a << request.stDeviceID.b);
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
			LOG(ERROR) << "ע���豸ʧ��:\r\n"
				<< "�豸IP:[" << device_->userIp_ << "]\r\n"
				<< "�豸�˿�:[" << device_->userPort_ << "]\r\n"
				<< "�û���:[" << device_->userName_ << "]\r\n"
				<< "����:[" << device_->loginPassword_ << "]\r\n"
				<< "������: " << NET_DVR_GetLastError();
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
