#include "hikcontroller.h"
#ifdef WINDOWS
#include <Windows.h>
#include <BaseTsd.h>
#include <WinNT.h>
#endif
#include "HCNetSDK.h"
#include "glog/logging.h"
#include<string>
#include <mutex>
//#include "camara.h"

extern "C" EXPORT CamaraController* GetController()
{
	LOG(INFO) << "Create HikDeviceProcesser";
	NET_DVR_Init();
	return new HikCamaraController;
}

HikCamaraController::HikCamaraController():
	login_(false), sdkusers_()
{
}

HikCamaraController::~HikCamaraController()
{
}

bool HikCamaraController::Login(std::string id, std::string ip, int port, std::string name, std::string pwd)
{
	NET_DVR_USER_LOGIN_INFO struLoginInfo = { 0 };
	struLoginInfo.bUseAsynLogin = false;
	memcpy(struLoginInfo.sDeviceAddress, ip.c_str(), NET_DVR_DEV_ADDRESS_MAX_LEN);
	memcpy(struLoginInfo.sUserName, name.c_str(), NAME_LEN);
	memcpy(struLoginInfo.sPassword, pwd.c_str(), PASSWD_LEN);
	struLoginInfo.wPort = port;

	NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = { 0 };

	LONG lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
	if (lUserID < 0)
	{
		LOG(ERROR) << "ע���豸ʧ��:\r\n"
			<< "�豸IP:[" << ip << "]\r\n"
			<< "�豸�˿�:[" << port << "]\r\n"
			<< "�û���:[" << name << "]\r\n"
			<< "����:[" << pwd << "]\r\n"
			<< "������: " << NET_DVR_GetLastError();
		return false;
	}
	
	LOG(INFO) << "Login " << id <<" : " << ip <<" successfully";

	NET_DVR_DEVICECFG_V40 devConfig;
	memset(&devConfig, 0, sizeof(devConfig));
	DWORD  dwBytesReturned = 0;
	BOOL ret = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_DEVICECFG_V40, 0, &devConfig, sizeof(devConfig), &dwBytesReturned);
	LOG(INFO) << "device type : " << devConfig.byDevTypeName;

	std::shared_ptr<SdkUser> user = std::make_shared<SdkUser>();
	user->SetUserId(lUserID);
	user->SetChannelId(0);
	user->Online(true); 
	user->deviceid_ = id;
	user->ip_ = ip;
	user->port_ = port;
	user->userName_ = name;
	user->password_ = pwd;
	sdkusers_.emplace(id, user);
	return true;
}

bool HikCamaraController::Login(std::shared_ptr<SdkUser> user)
{
	return Login(user->GetDeviceId(), user->ip_, user->port_, user->userName_, user->password_);
}

bool HikCamaraController::isLogin() const
{
	return false;
}

static bool DecodePTZCommand(const std::string& id, std::string sCode, DWORD &dwPTZCommand, DWORD &dwStop, DWORD &dwSpeed)
{

#define GB_PTZ_RIGHT           1            //��
#define GB_PTZ_LEFT            2            //��
#define GB_PTZ_DOWN            4            //��
#define GB_PTZ_UP              8            //��
#define GB_PTZ_ZOOM_BIG       16            //�Ŵ�
#define GB_PTZ_ZOOM_SMARLL    32            //��С

#define GB_PTZ_CIRCLE_SMARLL 0x48           //��Ȧ��С
#define GB_PTZ_CIRCLE_BIG    0x44           //��Ȧ���
#define GB_PTZ_CIRCLE_STOP   0x40           //��Ȧֹͣ
#define GB_PTZ_FOCUS_NER    0x42           //�����      
#define GB_PTZ_FOCUS_FAR    0x41           //����Զ
#define GB_PTZ_CIRCLE_FOCUS    0x49           //����Զ+��ȦС

#define GB_PTZ_PRESET_ADD     0x81           //����Ԥ��λ
#define GB_PTZ_PRESET_SET     0x82           //����Ԥ��λ
#define GB_PTZ_PRESET_DEL     0x83           //ɾ��Ԥ��λ

	int ispeed = 0;
	std::string  ptzcode = sCode;
	ptzcode = ptzcode.substr(6, 2);
	//int icmd = atoi(ptzcode.c_str());
	int icmd = 0;
	sscanf(ptzcode.c_str(), "%x", &icmd);

	if (icmd == 0)
	{
		dwStop = 1;
	}
	else
	{
		ptzcode = sCode.substr(8, 2);
		//int param5 = atoi(ptzcode.c_str());   //ˮƽ�ٶ�
		int param5;
		sscanf(ptzcode.c_str(), "%x", &param5);//ˮƽ�ٶ�

		ptzcode = sCode.substr(10, 2);
		//int param6 = atoi(ptzcode.c_str());    //�����ٶ�
		int param6;
		sscanf(ptzcode.c_str(), "%x", &param6);//�����ٶ�


		ptzcode = sCode.substr(12, 2);
		//int param7 = atoi(ptzcode.c_str());    //�����ٶ�
		int param7;
		sscanf(ptzcode.c_str(), "%x", &param7);//�����ٶ�

		switch (icmd)
		{
		case GB_PTZ_RIGHT:          //00000001
			dwPTZCommand = PAN_RIGHT;
			ispeed = param5 & 0xFF;
			break;
		case GB_PTZ_LEFT:          //00000010
			dwPTZCommand = PAN_LEFT;
			ispeed = param5 & 0xFF;
			break;
		case GB_PTZ_DOWN:          //00000100
			dwPTZCommand = TILT_DOWN;
			ispeed = param6 & 0xFF;
			break;
		case GB_PTZ_UP:          //00001000
			dwPTZCommand = TILT_UP;
			ispeed = param6 & 0xFF;
			break;
		case GB_PTZ_DOWN + GB_PTZ_RIGHT:          //00000101
			dwPTZCommand = DOWN_RIGHT;
			ispeed = param5 & 0xFF;
			break;
		case GB_PTZ_DOWN + GB_PTZ_LEFT:          //00000110
			dwPTZCommand = DOWN_LEFT;
			ispeed = param5 & 0xFF;
			break;
		case GB_PTZ_UP + GB_PTZ_RIGHT:          //00001001
			dwPTZCommand = UP_RIGHT;
			ispeed = param5 & 0xFF;
			break;
		case GB_PTZ_UP + GB_PTZ_LEFT:           //00001010
			dwPTZCommand = UP_LEFT;
			ispeed = param5 & 0xFF;
			break;
			/*************************��ͷ����*********************************/
		case GB_PTZ_ZOOM_BIG:                  //00010000 �䱶�Ŵ�
			dwPTZCommand = ZOOM_IN;
			ispeed = ((param7 & 0xF0) >> 4);
			break;
		case GB_PTZ_ZOOM_SMARLL:               //00100000 �䱶��С
			dwPTZCommand = ZOOM_OUT;
			ispeed = ((param7 & 0xF0) >> 4);
			break;
		case GB_PTZ_CIRCLE_SMARLL:              //0X48 ��Ȧ��С
			dwPTZCommand = IRIS_CLOSE;
			//m_pHikDevice->UpdateModel_LastFI(stsipid, IRIS_CLOSE);
			ispeed = param6 & 0xFF;
			break;
		case GB_PTZ_CIRCLE_BIG:                 // 0x44 ��Ȧ���         
			dwPTZCommand = IRIS_OPEN;
			//m_pHikDevice->UpdateModel_LastFI(stsipid, IRIS_OPEN);
			ispeed = param6 & 0xFF;
			break;
		case GB_PTZ_CIRCLE_STOP:                //0x40 ����\��Ȧֹͣ
		{
			//BaseModel model;
			//if (m_pHikDevice->GetModel(stsipid, model))
			//{
			//	dwPTZCommand = model.m_lastFI;
			//}
			dwPTZCommand = IRIS_OPEN;
			ispeed = 0;
		}

		break;
		case GB_PTZ_FOCUS_NER:                  //0x42 �����
			dwPTZCommand = FOCUS_NEAR;
			//m_pHikDevice->UpdateModel_LastFI(stsipid, FOCUS_NEAR);
			ispeed = param5 & 0xFF;
			break;
		case GB_PTZ_CIRCLE_FOCUS:                  //0x49 ����Զ+��ȦС  �豸��֧�ֵ�0x41����
		case GB_PTZ_FOCUS_FAR:                     //0x41 ����Զ
			dwPTZCommand = FOCUS_FAR;
			//m_pHikDevice->UpdateModel_LastFI(stsipid, FOCUS_FAR);
			ispeed = param5 & 0xFF;
			break;
		default:
			LOG(WARNING) << "PTZCommand: " << icmd << "dose not exist";
			/*************************��ͷ������С*********************************/
		}
	}
	//dwSpeed = ispeed*7/255;
	if (ispeed > 7)
	{
		dwSpeed = 7;
	}
	else if (ispeed <= 0)
	{
		dwSpeed = 1;
	}
	else
	{
		dwSpeed = ispeed;
	}

	return true;
}


static bool DecodePTZPreset(std::string sCode, DWORD& dwPTZPresetCmd, DWORD& dwPresetIndex)
{
	
#define GB_PTZ_PRESET_ADD     0x81           //����Ԥ��λ
#define GB_PTZ_PRESET_SET     0x82           //����Ԥ��λ
#define GB_PTZ_PRESET_DEL     0x83           //ɾ��Ԥ��λ

	std::string ptzcode = sCode;
	ptzcode = sCode.substr(6, 2);
	//int icmd = atoi(ptzcode.c_str());
	int icmd = 0;
	sscanf(ptzcode.c_str(), "%x", &icmd);

	ptzcode = sCode.substr(10, 2);
	sscanf(ptzcode.c_str(), "%x", &dwPresetIndex);
	dwPresetIndex = dwPresetIndex & 0xFF;

	switch (icmd)
	{
	case GB_PTZ_PRESET_ADD:
		dwPTZPresetCmd = SET_PRESET;
		break;
	case GB_PTZ_PRESET_SET:
		dwPTZPresetCmd = GOTO_PRESET;

		break;
	case GB_PTZ_PRESET_DEL:
		dwPTZPresetCmd = CLE_PRESET;
		break;
	default:
		LOG(WARNING) << "PTZCommand: " << icmd << "dose not exist";
		//LOG_WARN(g_nLogID, "PTZCommand[" << icmd << "]  PTZ_PRESET is not exist");
	}

	return true;

}

static bool DecodePTZCruise(std::string scode, DWORD &dwPTZCruiseCmd, BYTE &byCruiseRoute, BYTE &byCruisePoint, WORD &wInput)
{

#define GB_PTZ_CRUISE_ADD      0x84           //����Ѳ����
#define GB_PTZ_CRUISE_DLL      0x85           //ɾ��Ѳ����
#define GB_PTZ_CRUISE_SPEED    0x86           //����Ѳ���ٶ�
#define GB_PTZ_CRUISE_DWELL    0x87          //����Ѳ��ͣ��ʱ��
#define GB_PTZ_CRUISE_START    0x88           //��ʼѲ��

	std::string ptzcode = scode;
	ptzcode = scode.substr(6, 2);
	//int icmd = atoi(ptzcode.c_str());
	int icmd = 0;
	sscanf(ptzcode.c_str(), "%x", &icmd);

	ptzcode = scode.substr(8, 2);
	//int param5 = atoi(ptzcode.c_str());
	int param5 = 0;
	sscanf(ptzcode.c_str(), "%x", &param5);

	ptzcode = scode.substr(10, 2);
	//int param6 = atoi(ptzcode.c_str());
	int param6 = 0;
	sscanf(ptzcode.c_str(), "%x", &param6);

	ptzcode = scode.substr(12, 2);
	//int param7 = atoi(ptzcode.c_str());
	int param7 = 0;
	sscanf(ptzcode.c_str(), "%x", &param7);
	int tmp = 0;
	switch (icmd)
	{
	case GB_PTZ_CRUISE_ADD:               //0x84 ����Ѳ����
		dwPTZCruiseCmd = FILL_PRE_SEQ;
		byCruiseRoute = param5 & 0xFF;
		byCruisePoint = param6 & 0xFF;
		wInput = param6 & 0xFF;
		break;
	case GB_PTZ_CRUISE_DLL:               //0x85 ɾ��Ѳ����
		dwPTZCruiseCmd = CLE_PRE_SEQ;
		byCruiseRoute = param5 & 0xFF;
		byCruisePoint = param6 & 0xFF;
		wInput = param6 & 0xFF;
		break;
	case GB_PTZ_CRUISE_SPEED:               //0x86 ����Ѳ���ٶ�
		dwPTZCruiseCmd = SET_SEQ_SPEED;
		byCruiseRoute = param5 & 0xFF;
		tmp = param6 & 0xFF;
		wInput = (((param7 & 0xF0) << 4) + tmp) * 40 / 4095;  //�����ٶ����40  ������� 0xfff
		if (wInput == 0)
		{
			wInput = 1;
		}
		break;
	case GB_PTZ_CRUISE_DWELL:               //0x87 ����Ѳ��ͣ��ʱ��
		dwPTZCruiseCmd = SET_SEQ_DWELL;
		byCruiseRoute = param5 & 0xFF;
		tmp = param6 & 0xFF;
		wInput = (((param7 & 0xF0) << 4) + tmp) * 255 / 4095;  //�����ٶ����40  ������� 0xfff
		if (wInput == 0)
		{
			wInput = 1;
		}
		break;
	case GB_PTZ_CRUISE_START:               //0x88 ��ʼѲ��  ֹͣѲ����PTZֹͣ����
		dwPTZCruiseCmd = RUN_SEQ;
		byCruiseRoute = param5 & 0xFF;
		break;
	default:
		LOG(WARNING) << "PTZCommand: " << icmd << "dose not exist";
		//LOG_WARN(g_nLogID, "PTZCommand[" << icmd << "]  PTZ_CRUISE is not exist");
	}

	return true;
}


void HikCamaraController::ptzControl(std::string id, std::string cmd)
{
	LOG(INFO) << "HikCamaraController ptzControl";


	auto search = sdkusers_.find(id);
	if (search == sdkusers_.end())
	{
		if (!Login(search->second))
		{
			return;
		}
		
	}
	else
	{
		std::string ptzcode = cmd.substr(6, 2);
		int icmd = atoi(ptzcode.c_str());

		if (icmd < 0x4A)
		{
			DWORD dwPTZCommand = PAN_RIGHT;//��ʼ����ֵ�������ֹͣ����dwPTZCommand������ֵ
			DWORD dwStop = 0;//1ֹͣ��̨����
			DWORD dwSpeed = 1;//�ٶȡ�1-7��


			if (!DecodePTZCommand(id, cmd, dwPTZCommand, dwStop, dwSpeed))
			{
				LOG(ERROR) << "������̨�������,SIPID:" << id;
				return;
			}
			//NET_DVR_PTZControlWithSpeed(Model.m_nRealHandle, dwPTZCommand, dwStop, dwSpeed))
			if (!NET_DVR_PTZControlWithSpeed_Other(search->second->GetUserId(), 1, dwPTZCommand, dwStop, dwSpeed))
			{
				LOG(ERROR) << "��̨���Ƴ���,SIPID:" << id << "����ԭ��:" << NET_DVR_GetLastError();
				return;
			}
			LOG(INFO) << "����ptz�ɹ�,SIPID:" << id << " cmd = " << icmd << "  HKcmd =" << dwPTZCommand << " stop=" << dwStop << " speed=" << dwSpeed;
		}
		else if ((0x80 < icmd) && (icmd < 0x84))
		{
			DWORD   dwPTZPresetCmd = SET_PRESET;
			DWORD   dwPresetIndex = 0;

			if (!DecodePTZPreset(cmd, dwPTZPresetCmd, dwPresetIndex))
			{
				LOG(ERROR) << "������̨Ԥ�õ��������,SIPID:" <<id;
				return;
			}

			if (!NET_DVR_PTZPreset_Other(search->second->GetUserId(), 1, dwPTZPresetCmd, dwPresetIndex))
			{

				LOG(ERROR) << "������̨Ԥ�õ��������,SIPID:" << id << "����ԭ��:" << NET_DVR_GetLastError();
				return;
			}
			LOG(INFO) << "����Ԥ�õ�ɹ�,SIPID:" << id << " cmd = " << icmd << "  HKcmd =" << dwPTZPresetCmd << " INDEX=" << dwPresetIndex;
		}
		else if ((0x83 < icmd) && (icmd < 0x89))
		{
			DWORD   dwPTZCruiseCmd = SET_PRESET;
			BYTE    byCruiseRoute = 0;
			BYTE    byCruisePoint = 0;
			WORD    wInput = 0;

			if (!DecodePTZCruise(cmd, dwPTZCruiseCmd, byCruiseRoute, byCruisePoint, wInput))
			{
				LOG(ERROR) << "������̨Ԥ�õ��������,SIPID:" << id;

				return;
			}

			if (!NET_DVR_PTZCruise_Other(search->second->GetUserId(), 1, dwPTZCruiseCmd, byCruiseRoute, byCruisePoint, wInput))
			{
				LOG(ERROR) << "������̨Cruise�������,SIPID:" << id << "����ԭ��:" << NET_DVR_GetLastError();

				return;
			}
			LOG(INFO) << "����Cruise�ɹ�,SIPID:" << id << " cmd = " << icmd << "  HKcmd =" << dwPTZCruiseCmd << " rute=" << byCruiseRoute << " point=" << byCruisePoint << "  input=" << wInput;
		}
		else if ((0x88 < icmd) && (icmd < 0x8B))
		{
			LOG(WARNING) << "������̨ɨ�躣����֧��,SIPID:" << id << " cmd = " << icmd;
		}
		else
		{
			LOG(WARNING) << "������̨������귶Χ,SIPID:" << id << " cmd = " << icmd;
		}
	}

	return;

}

void HikCamaraController::getDeviceStatusAsync(std::string id, std::function<void(int)> cb)
{
	auto search = sdkusers_.find(id);
	if (search == sdkusers_.end())
	{
		if (!Login(search->second))
		{
			cb(0);
			return;
		}
	}
	else
	{
		NET_DVR_WORKSTATE_V40 *pStruWorkStateV40 = new NET_DVR_WORKSTATE_V40;
		DWORD dwList = 0;
		memset(pStruWorkStateV40, 0, sizeof(NET_DVR_WORKSTATE_V40));
		NET_DVR_GETWORKSTATE_COND struWorkStateCond = { 0 };
		struWorkStateCond.dwSize = sizeof(NET_DVR_GETWORKSTATE_COND);
		struWorkStateCond.byFindChanByCond = 0;
		struWorkStateCond.byFindHardByCond = 0;
		if (!NET_DVR_GetDeviceConfig(search->second->GetUserId(), NET_DVR_GET_WORK_STATUS, 1, \
			&struWorkStateCond, sizeof(NET_DVR_GETWORKSTATE_COND), &dwList, pStruWorkStateV40, sizeof(NET_DVR_WORKSTATE_V40)) || (dwList != 0))
		{
			cb(0);
			return;
		}

		if (0 == pStruWorkStateV40->dwDeviceStatic)
		{
			cb(0);
		}
		else
		{
			cb(1);
		}
	}
}

void HikCamaraController::getDeviceStatus(std::string id, dt::DeviceStatus & st)
{
	auto search = sdkusers_.find(id);
	if (search == sdkusers_.end())
	{
		if (!Login(search->second))
		{
			//cb(0);
			st.status = 0;
			return;
		}
	}
	else
	{
		NET_DVR_WORKSTATE_V40 *pStruWorkStateV40 = new NET_DVR_WORKSTATE_V40;
		DWORD dwList = 0;
		memset(pStruWorkStateV40, 0, sizeof(NET_DVR_WORKSTATE_V40));
		NET_DVR_GETWORKSTATE_COND struWorkStateCond = { 0 };
		struWorkStateCond.dwSize = sizeof(NET_DVR_GETWORKSTATE_COND);
		struWorkStateCond.byFindChanByCond = 0;
		struWorkStateCond.byFindHardByCond = 0;
		if (!NET_DVR_GetDeviceConfig(search->second->GetUserId(), NET_DVR_GET_WORK_STATUS, 1, \
			&struWorkStateCond, sizeof(NET_DVR_GETWORKSTATE_COND), &dwList, pStruWorkStateV40, sizeof(NET_DVR_WORKSTATE_V40)) || (dwList != 0))
		{
			st.status = 0;
			return;
		}

		if (0 == pStruWorkStateV40->dwDeviceStatic)
		{
			st.status = 0;
		}
		else
		{
			st.status = 1;
		}
	}
}

void HikCamaraController::processData(long hd, char *data , uint32_t len)
{
	// streamMutex_.lock();
	// auto cb = cbMap_.find(hd);
	// if (cb != cbMap_.end())
	// {
	// 	streamMutex_.unlock();
	// 	cb->second(data, len);
	// }
	// else
	// {
	// 	streamMutex_.lock();
	// 	return;
	// }
}

bool HikCamaraController::logIn(std::string id, std::string ip, int port, std::string userName, std::string pwd)
{
	NET_DVR_USER_LOGIN_INFO struLoginInfo = { 0 };
	struLoginInfo.bUseAsynLogin = false;
	memcpy(struLoginInfo.sDeviceAddress, ip.c_str(), NET_DVR_DEV_ADDRESS_MAX_LEN);
	memcpy(struLoginInfo.sUserName, userName.c_str(), NAME_LEN);
	memcpy(struLoginInfo.sPassword, pwd.c_str(), PASSWD_LEN);
	struLoginInfo.wPort = port;

	NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = { 0 };

	LONG lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
	if (lUserID < 0)
	{
		LOG(ERROR) << "ע���豸ʧ��:\r\n"
			<< "�豸IP:[" << ip << "]\r\n"
			<< "�豸�˿�:[" << port << "]\r\n"
			<< "�û���:[" << userName << "]\r\n"
			<< "����:[" << pwd << "]\r\n"
			<< "������: " << NET_DVR_GetLastError();
		return false;
	}

	LOG(INFO) << "Login " << ip << " successfully";

	NET_DVR_DEVICECFG_V40 devConfig;
	memset(&devConfig, 0, sizeof(devConfig));
	DWORD  dwBytesReturned = 0;
	BOOL ret = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_DEVICECFG_V40, 0, &devConfig, sizeof(devConfig), &dwBytesReturned);
	LOG(INFO) << "device type : " << devConfig.byDevTypeName;

	std::shared_ptr<SdkUser> user = std::make_shared<SdkUser>();
	user->SetUserId(lUserID);
	user->SetChannelId(0);
	user->Online(true);
	user->deviceid_ = id;
	user->ip_ = ip;
	user->port_ = port;
	user->userName_ =userName;
	user->password_ = pwd;
	sdkusers_.emplace(id, user);
	return true;
}

static void __stdcall RealDataCallBack(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser)
{
	//std::cout<<"receive data: "<< dwBufSize <<"   type: "<< dwDataType <<std::endl;
	if (pUser == nullptr)
	{
		return;
	}
	HikCamaraController * controller = (HikCamaraController*)pUser;
	//std::shared_ptr<PSBuffer> buf = ((PSBuffer *)pUser)->getPtr();
	char *id = (char *)pUser;
	switch (dwDataType)
	{
	case NET_DVR_SYSHEAD:
		break;

	case NET_DVR_STREAMDATA:
	{
		auto start = std::chrono::high_resolution_clock::now();
		//StreamManager::getInstance()->putDatatoBuffer("60000000001310001430", pBuffer, dwBufSize);
		controller->processData(lPlayHandle, (char *)pBuffer, dwBufSize);
		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		//LOG(INFO) << "elapse  " << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << " microseconds";
	}
	break;
	default:
		break;
	}
}

static void __stdcall newRealDataCallBack(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser)
{
	if (pUser == nullptr)
	{
		return;
	}

	std::function<void(char *, uint32_t)> *cb = (std::function<void(char *, uint32_t)> *)pUser;
	switch (dwDataType)
	{
	case NET_DVR_SYSHEAD:
		break;

	case NET_DVR_STREAMDATA:
	{
		auto start = std::chrono::high_resolution_clock::now();
		//StreamManager::getInstance()->putDatatoBuffer("60000000001310001430", pBuffer, dwBufSize);
		//controller->processData(lPlayHandle, (char *)pBuffer, dwBufSize);
		(*cb)((char *)pBuffer, dwBufSize);
		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		//LOG(INFO) << "elapse  " << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << " microseconds";
	}
	break;
	default:
		break;
	}
}

//bool HikCamaraController::openRealStream(const dt::OpenRealStream& param)
bool HikCamaraController::openRealStream(std::string id, std::string ip, int port, std::string user,
	std::string pwd, std::function<void(char *, uint32_t)> datacb)
{
	auto search = sdkusers_.find(id);
	if (search == sdkusers_.end())
	{
		if (!logIn(id, ip, port, user, pwd))
		{
			LOG(ERROR) << "Hik sdk login camara failed when opening stream";
			return false;
		}
	}
	LOG(ERROR) << "Hik sdk login camara {"<< ip<<" : "<<port<<"} successfully!";
	
	auto sdkuser = sdkusers_.find(id);
	if (sdkuser == sdkusers_.end())
	{
		LOG(ERROR) << "Hik opening stream failed";
		return false;
	}

	dataCallBack *dtcb = new dataCallBack(datacb);
	//sdkuser->second->SetCallBack(dtcb);
	//HWND hWnd = NULL;
	NET_DVR_PREVIEWINFO struPlayInfo = { 0 };
#if (defined(_WIN32) || defined(_WIN_WCE))
	struPlayInfo.hPlayWnd = NULL;
#elif defined(__linux__)
	struPlayInfo.hPlayWnd = 0;
#endif
	//struPlayInfo.hPlayWnd = NULL; //��Ҫ SDK ����ʱ�����Ϊ��Чֵ����ȡ��������ʱ����Ϊ��
	struPlayInfo.lChannel = 1;//
	struPlayInfo.dwLinkMode = 0;//0- TCP ��ʽ�� 1- UDP ��ʽ�� 2- �ಥ��ʽ�� 3- RTP ��ʽ�� 4-RTP/RTSP�� 5-RSTP/HTTP
	struPlayInfo.dwStreamType = 0;//0-�������� 1-�������� 2-���� 3�� 3-���� 4���Դ�����
									//	
	struPlayInfo.byPreviewMode = 0;//����Ԥ��
	struPlayInfo.bBlocked = 1;//0- ������ȡ���� 1- ����ȡ��
	struPlayInfo.bPassbackRecord = 0;//������¼��ش�;
	struPlayInfo.byProtoType = 0;//˽��Э��
								

	//long playHandle_ = NET_DVR_RealPlay_V40(sdkuser->second->GetUserId(), &struPlayInfo, RealDataCallBack, (void *)this);
	long playHandle_ = NET_DVR_RealPlay_V40(sdkuser->second->GetUserId(), &struPlayInfo, newRealDataCallBack, dtcb);
	if (playHandle_ < 0)
	{
		LOG(ERROR) << "Hik sdk open stream failed, error code: " << NET_DVR_GetLastError();
		return false;
	}
	else
	{
		//int iRet = NET_DVR_SetRealDataCallBack(playHandle_, RealDataCallBack, this);
		sdkuser->second->SetPlayHandle(playHandle_);

		std::lock_guard<std::mutex> lk(streamMutex_);
		//cbMap_.emplace(playHandle_, datacb);
		cbMap_.emplace(playHandle_, dtcb);
		LOG(INFO) << "Hik sdk open stream successfully!";
		return true;
	}
}

bool HikCamaraController::closeRealStream(const std::string & id)
{
	auto search = sdkusers_.find(id);
	if (search == sdkusers_.end())
	{
		LOG(ERROR) << "Hik sdk close stream failed: not find camara " << id;
		return false;
	}
	auto sdksuer = search->second;
	LOG(ERROR) << "hik sdk PlayHandle: " << sdksuer->GetPlayHandle();

	int ret = NET_DVR_SetRealDataCallBack(search->second->GetPlayHandle(), NULL, 0);
	if(!ret)
	{
		LOG(ERROR) << "NET_DVR_SetRealDataCallBack error: " << NET_DVR_GetLastError();
	}

	if(!NET_DVR_StopRealPlay(search->second->GetPlayHandle()))
	{
		LOG(ERROR) << "Hik sdk close stream failed, error code: " << NET_DVR_GetLastError();
	}

	std::lock_guard<std::mutex> lk(streamMutex_);
	auto cb = cbMap_.find(search->second->GetPlayHandle());
	if (cb == cbMap_.end())
	{
		return true;
	}

	auto c = cb->second;
	delete c;
	cbMap_.erase(cb);

	return true;
}
