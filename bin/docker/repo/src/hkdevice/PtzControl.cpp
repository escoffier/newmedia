#include "PtzControl.h"
#include "HCNetSDK.h"
#include "glog\logging.h"
#include "SdkUsers.h"
PtzControl::PtzControl(std::shared_ptr<Device> device, std::string ptzcmd, std::function<void(std::string, int )> response):
	AbstractOperation(device), ptzcmd_(ptzcmd), response_(response)
{
}

PtzControl::~PtzControl()
{
}

void PtzControl::Process()
{
	std::string ptzcode = ptzcmd_.substr(6, 2);
	//int   icmd = atoi(ptzcode.c_str());
	int icmd = 0;
	icmd = sscanf_s(ptzcode.c_str(), "%x", &icmd);
	long userId = SdkUserManager::GetInstance()->GetSdkUser(device_->userId_)->GetUserId();
	long channelId = SdkUserManager::GetInstance()->GetSdkUser(device_->userId_)->GetChannelId();
	if (icmd < 0x4A)
	{
		DWORD dwPTZCommand = PAN_RIGHT;//初始给个值，如果是停止操作dwPTZCommand随便给个值
		DWORD dwStop = 0;//1停止云台操作
		DWORD dwSpeed = 1;//速度【1-7】


		if (!DecodePTZCommand(device_->userId_, ptzcmd_, dwPTZCommand, dwStop, dwSpeed))
		{
			//request.outputparam.pthis = m_pHikDevice->m_pRecvResponseThis;
			//request.outputparam.nErrorID = G_ERROR_PTZCOMMAND_FAIL;
			//m_pHikDevice->m_pRecvResponse(request);
			if (response_ != nullptr)
			{
				response_(device_->userId_, -1);
			}
			LOG(WARNING)<<"解析云台命令出错,SIPID:" << device_->userId_;
			return;
		}
		//NET_DVR_PTZControlWithSpeed(Model.m_nRealHandle, dwPTZCommand, dwStop, dwSpeed))
		if (!NET_DVR_PTZControlWithSpeed_Other(userId, channelId, dwPTZCommand, dwStop, dwSpeed))
		{
			if (response_ != nullptr)
			{
				response_(device_->userId_, -1);
			}
			LOG(WARNING) <<  "云台控制出错,SIPID:" << device_->userId_ << "错误原因:" << NET_DVR_GetLastError();
			return;
		}
		LOG(INFO) <<  "设置ptz成功,SIPID:" << device_->userId_ << " cmd = " << icmd << "  HKcmd =" << dwPTZCommand << " stop=" << dwStop << " speed=" << dwSpeed;

	}
	else if ((0x80 < icmd) && (icmd < 0x84))
	{
		DWORD   dwPTZPresetCmd = SET_PRESET;
		DWORD   dwPresetIndex = 0;

		if (!DecodePTZPreset(ptzcmd_, dwPTZPresetCmd, dwPresetIndex))
		{
			if (response_ != nullptr)
			{
				response_(device_->userId_, -1);
			}
			LOG(WARNING) << "解析云台预置点命令出错,SIPID:" << device_->userId_;

			return;
		}

		if (!NET_DVR_PTZPreset_Other(userId, channelId, dwPTZPresetCmd, dwPresetIndex))
		{
			if (response_ != nullptr)
			{
				response_(device_->userId_, -1);
			}
			LOG(WARNING) << "设置云台预置点命令出错,SIPID:" << device_->userId_ << "错误原因:" << NET_DVR_GetLastError();
			return ;
		}
		LOG(INFO) << "设置预置点成功,SIPID:" << device_->userId_ << " cmd = " << icmd << "  HKcmd =" << dwPTZPresetCmd << " INDEX=" << dwPresetIndex;
	}
	else if ((0x83 < icmd) && (icmd < 0x89))
	{
		DWORD   dwPTZCruiseCmd = SET_PRESET;
		BYTE    byCruiseRoute = 0;
		BYTE    byCruisePoint = 0;
		WORD    wInput = 0;

		if (!DecodePTZCruise(ptzcmd_, dwPTZCruiseCmd, byCruiseRoute, byCruisePoint, wInput))
		{
			if (response_ != nullptr)
			{
				response_(device_->userId_, -1);
			}
			LOG(WARNING) << "解析云台预置点命令出错,SIPID:" << device_->userId_;

			return ;
		}

		if (!NET_DVR_PTZCruise_Other(userId, channelId, dwPTZCruiseCmd, byCruiseRoute, byCruisePoint, wInput))
		{
			if (response_ != nullptr)
			{
				response_(device_->userId_, -1);
			}
			LOG(WARNING) << "设置云台Cruise命令出错,SIPID:" << device_->userId_ << "错误原因:" << NET_DVR_GetLastError();

			return ;
		}
		LOG(INFO) << "设置Cruise成功,SIPID:" << device_->userId_ << " cmd = " << icmd << "  HKcmd =" << dwPTZCruiseCmd << " rute=" << byCruiseRoute << " point=" << byCruisePoint << "  input=" << wInput;
	}
	else if ((0x88 < icmd) && (icmd < 0x8B))
	{
		LOG(WARNING) << "设置云台扫描海康不支持,SIPID:" << device_->userId_ << " cmd = " << icmd;
	}
	else
	{
		LOG(WARNING) << "设置云台命令超国标范围,SIPID:" << device_->userId_ << " cmd = " << icmd;
	}


	if (response_ != nullptr)
	{
		response_(device_->userId_, -1);
	}
	//request.outputparam.pthis = m_pHikDevice->m_pRecvResponseThis;
	//request.outputparam.nErrorID = 0;
	//m_pHikDevice->m_pRecvResponse(request);
	
	return;
}

bool PtzControl::DecodePTZCommand(const std::string & stsipid, std::string sCode, DWORD & dwPTZCommand, DWORD & dwStop, DWORD & dwSpeed)
{

#define GB_PTZ_RIGHT           1            //右
#define GB_PTZ_LEFT            2            //左
#define GB_PTZ_DOWN            4            //下
#define GB_PTZ_UP              8            //上
#define GB_PTZ_ZOOM_BIG       16            //放大
#define GB_PTZ_ZOOM_SMARLL    32            //缩小

#define GB_PTZ_CIRCLE_SMARLL 0x48           //光圈缩小
#define GB_PTZ_CIRCLE_BIG    0x44           //光圈变大
#define GB_PTZ_CIRCLE_STOP   0x40           //光圈停止
#define GB_PTZ_FOCUS_NER    0x42           //焦点近      
#define GB_PTZ_FOCUS_FAR    0x41           //焦点远
#define GB_PTZ_CIRCLE_FOCUS    0x49           //焦点远+光圈小

#define GB_PTZ_PRESET_ADD     0x81           //设置预置位
#define GB_PTZ_PRESET_SET     0x82           //调用预置位
#define GB_PTZ_PRESET_DEL     0x83           //删除预置位

	int ispeed = 0;
	std::string ptzcode = sCode;
	ptzcode = ptzcode.substr(6, 2);
	//int icmd = atoi(ptzcode.c_str());
	int icmd = 0;
	sscanf_s(ptzcode.c_str(), "%x", &icmd);

	if (icmd == 0)
	{
		dwStop = 1;
	}
	else
	{
		ptzcode = sCode.substr(8, 2);
		//int param5 = atoi(ptzcode.c_str());   //水平速度
		int param5;
		sscanf_s(ptzcode.c_str(), "%x", &param5);//水平速度

		ptzcode = sCode.substr(10, 2);
		//int param6 = atoi(ptzcode.c_str());    //上下速度
		int param6;
		sscanf_s(ptzcode.c_str(), "%x", &param6);//上下速度


		ptzcode = sCode.substr(12, 2);
		//int param7 = atoi(ptzcode.c_str());    //缩放速度
		int param7;
		sscanf_s(ptzcode.c_str(), "%x", &param7);//缩放速度

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
			/*************************镜头倍率*********************************/
		case GB_PTZ_ZOOM_BIG:                  //00010000 变倍放大
			dwPTZCommand = ZOOM_IN;
			ispeed = ((param7 & 0xF0) >> 4);
			break;
		case GB_PTZ_ZOOM_SMARLL:               //00100000 变倍缩小
			dwPTZCommand = ZOOM_OUT;
			ispeed = ((param7 & 0xF0) >> 4);
			break;
		case GB_PTZ_CIRCLE_SMARLL:              //0X48 光圈缩小
			dwPTZCommand = IRIS_CLOSE;
			//m_pHikDevice->UpdateModel_LastFI(stsipid, IRIS_CLOSE);
			ispeed = param6 & 0xFF;
			break;
		case GB_PTZ_CIRCLE_BIG:                 // 0x44 光圈变大         
			dwPTZCommand = IRIS_OPEN;
			//m_pHikDevice->UpdateModel_LastFI(stsipid, IRIS_OPEN);
			ispeed = param6 & 0xFF;
			break;
		case GB_PTZ_CIRCLE_STOP:                //0x40 焦点\光圈停止
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
		case GB_PTZ_FOCUS_NER:                  //0x42 焦点近
			dwPTZCommand = FOCUS_NEAR;
			//m_pHikDevice->UpdateModel_LastFI(stsipid, FOCUS_NEAR);
			ispeed = param5 & 0xFF;
			break;
		case GB_PTZ_CIRCLE_FOCUS:                  //0x49 焦点远+光圈小  设备不支持当0x41处理
		case GB_PTZ_FOCUS_FAR:                     //0x41 焦点远
			dwPTZCommand = FOCUS_FAR;
			//m_pHikDevice->UpdateModel_LastFI(stsipid, FOCUS_FAR);
			ispeed = param5 & 0xFF;
			break;
		default:
			LOG(INFO)<<"PTZCommand[" << icmd << "] is not exist";
			/*************************镜头倍率缩小*********************************/
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

bool PtzControl::DecodePTZCruise(std::string scode, DWORD &dwPTZCruiseCmd, BYTE &byCruiseRoute, BYTE &byCruisePoint, WORD &wInput)
{
#define GB_PTZ_CRUISE_ADD      0x84           //加入巡航点
#define GB_PTZ_CRUISE_DLL      0x85           //删除巡航点
#define GB_PTZ_CRUISE_SPEED    0x86           //设置巡航速度
#define GB_PTZ_CRUISE_DWELL    0x87          //设置巡航停留时间
#define GB_PTZ_CRUISE_START    0x88           //开始巡航

	std::string ptzcode = scode;
	ptzcode = scode.substr(6, 2);
	//int icmd = atoi(ptzcode.c_str());
	int icmd = 0;
	sscanf_s(ptzcode.c_str(), "%x", &icmd);

	ptzcode = scode.substr(8, 2);
	//int param5 = atoi(ptzcode.c_str());
	int param5 = 0;
	sscanf_s(ptzcode.c_str(), "%x", &param5);

	ptzcode = scode.substr(10, 2);
	//int param6 = atoi(ptzcode.c_str());
	int param6 = 0;
	sscanf_s(ptzcode.c_str(), "%x", &param6);

	ptzcode = scode.substr(12, 2);
	//int param7 = atoi(ptzcode.c_str());
	int param7 = 0;
	sscanf_s(ptzcode.c_str(), "%x", &param7);
	int tmp = 0;
	switch (icmd)
	{
	case GB_PTZ_CRUISE_ADD:               //0x84 加入巡航点
		dwPTZCruiseCmd = FILL_PRE_SEQ;
		byCruiseRoute = param5 & 0xFF;
		byCruisePoint = param6 & 0xFF;
		wInput = param6 & 0xFF;
		break;
	case GB_PTZ_CRUISE_DLL:               //0x85 删除巡航点
		dwPTZCruiseCmd = CLE_PRE_SEQ;
		byCruiseRoute = param5 & 0xFF;
		byCruisePoint = param6 & 0xFF;
		wInput = param6 & 0xFF;
		break;
	case GB_PTZ_CRUISE_SPEED:               //0x86 设置巡航速度
		dwPTZCruiseCmd = SET_SEQ_SPEED;
		byCruiseRoute = param5 & 0xFF;
		tmp = param6 & 0xFF;
		wInput = (((param7 & 0xF0) << 4) + tmp) * 40 / 4095;  //海康速度最大40  国标最大 0xfff
		if (wInput == 0)
		{
			wInput = 1;
		}
		break;
	case GB_PTZ_CRUISE_DWELL:               //0x87 设置巡航停留时间
		dwPTZCruiseCmd = SET_SEQ_DWELL;
		byCruiseRoute = param5 & 0xFF;
		tmp = param6 & 0xFF;
		wInput = (((param7 & 0xF0) << 4) + tmp) * 255 / 4095;  //海康速度最大40  国标最大 0xfff
		if (wInput == 0)
		{
			wInput = 1;
		}
		break;
	case GB_PTZ_CRUISE_START:               //0x88 开始巡航  停止巡航用PTZ停止代替
		dwPTZCruiseCmd = RUN_SEQ;
		byCruiseRoute = param5 & 0xFF;
		break;
	default:
		LOG(WARNING)<< "PTZCommand[" << icmd << "]  PTZ_CRUISE is not exist";
	}
	return true;
}

bool  PtzControl::DecodePTZPreset(std::string sCode, DWORD& dwPTZPresetCmd, DWORD& dwPresetIndex)
{
#define GB_PTZ_PRESET_ADD     0x81           //设置预置位
#define GB_PTZ_PRESET_SET     0x82           //调用预置位
#define GB_PTZ_PRESET_DEL     0x83           //删除预置位

	std::string ptzcode = sCode;
	ptzcode = sCode.substr(6, 2);
	//int icmd = atoi(ptzcode.c_str());
	int icmd = 0;
	sscanf_s(ptzcode.c_str(), "%x", &icmd);

	ptzcode = sCode.substr(10, 2);
	sscanf_s(ptzcode.c_str(), "%x", &dwPresetIndex);
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
		LOG(WARNING)<< "PTZCommand[" << icmd << "]  PTZ_PRESET is not exist";
	}

	
	return true;

}