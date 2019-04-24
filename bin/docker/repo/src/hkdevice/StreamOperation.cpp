#include "StreamOperation.h"
#include "HCNetSDK.h"
#include "glog\logging.h"
#include "SdkUsers.h"

//StreamOperation::StreamOperation(std::shared_ptr<Device> device, std::function<void(std::string, int)> response, std::function<void(/*std::shared_ptr<Device> device,*/std::string id, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize)> dataproc):
//	AbstractOperation(device) ,response_(response), dataProc_(dataproc)
//{
//}
StreamOperation::~StreamOperation()
{
}

static void RealDataCallBack(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser)
{

	if (pUser == nullptr)
	{
		return;
	}
	SdkUser *sdkuser = (SdkUser *)pUser;
	//sdkuser->DataProcFunc()( sdkuser->GetDeviceId(), dwDataType, pBuffer, dwBufSize);
	//DataProc *dataProc = (DataProc *)pUser;
	//(*dataProc)(dataProc->deviceId_, dwDataType, pBuffer, dwBufSize);
}

void StreamOperation::Process()
{
	std::shared_ptr<SdkUser> sdkUser = SdkUserManager::GetInstance()->GetSdkUser(device_->userId_);
	if (sdkUser == nullptr)
	{
		LOG(ERROR) << "不存在的设备"<< device_->userId_;
		return;
	}
	if ( ! (sdkUser->IsOnline()) )
	{
		LOG(ERROR) << "设备: " << device_->userId_<<"没有登录";
		response_(device_->userId_, -1);
		return;
	}

	//DataProc *dataProc = nullptr;
	if (sdkUser->IsPlaying())
	{
		//sdkUser->IncrePlayCount();
		response_(device_->userId_, 0);
		LOG(ERROR)<< "实时预览已开启,SIPID:" << device_->userId_;
		return ;
	}

	HWND hWnd = NULL;
	//= GetConsoleWindow();
	NET_DVR_PREVIEWINFO struPlayInfo = { 0 };
	struPlayInfo.hPlayWnd = hWnd;
	struPlayInfo.lChannel = 1;//request.ulChannelID;//iChanIndex+g_struDeviceInfo[iDeviceIndex].iStartChan;
	struPlayInfo.dwLinkMode = 0;//Tcp
	struPlayInfo.dwStreamType = 0;//主码流
								  //	struPlayInfo.sMultiCastIP = g_struDeviceInfo[iDeviceIndex].chDeviceMultiIP;
	struPlayInfo.byPreviewMode = 0;//正常预览
	struPlayInfo.bBlocked = 1;//阻塞取流
	struPlayInfo.bPassbackRecord = 0;//不启用录像回传;
	struPlayInfo.byProtoType = 0;//私有协议
	LONG lPlayHandle = 0;
	sdkUser->SetDeviceId(device_->userId_);
	//sdkUser->SetDataProc(dataProc_);


	//当一个设备退出登录后，智能指针管理的sdkUser对象将销毁，理论上SDK将不会再调用回调函数RealDataCallBack
	lPlayHandle = NET_DVR_RealPlay_V40( sdkUser->GetUserId(), &struPlayInfo, RealDataCallBack, (void *)sdkUser.get());
	if (-1 == lPlayHandle)
	{
		LOG(ERROR)<< "实时呼叫设备失败:\r\n"
			<< "设备ID:[" << device_->userId_ << "]\r\n"
			<< "设备通道号:[" << struPlayInfo.lChannel << "]\r\n"
			<< "失败原因值:[" << NET_DVR_GetLastError() << "]\r\n";

		response_(device_->userId_, -1);
		return ;
	}
	else
	{
		//dataProc->usedCount.fetch_add(1);
		sdkUser->SetPlayHandle(lPlayHandle);
		//sdkUser->IncrePlayCount();
		response_(device_->userId_, 0);
		return ;
	}


}


StopStream::StopStream(std::string id, std::function<void(std::string, int)> response):
	deviceId_(id), response_(response)
{
}

StopStream::~StopStream()
{
}

void StopStream::Process()
{
	std::shared_ptr<SdkUser> sdkUser = SdkUserManager::GetInstance()->GetSdkUser(deviceId_);
	if (sdkUser == nullptr)
	{
		LOG(ERROR) << "不存在的设备" << deviceId_;
		return;
	}
	if (!(sdkUser->IsOnline()))
	{
		LOG(ERROR) << "设备: " << deviceId_ << "没有登录";
		response_(deviceId_, -1);
		return;
	}

	//sdkUser->DecrePlayCount();
	if ( !sdkUser->IsPlaying())
	{
		if (!NET_DVR_StopRealPlay(sdkUser->GetPlayHandle()))
		{
			//request.outputparam.pthis = m_pHikDevice->m_pRecvResponseThis;
			//request.outputparam.nErrorID = NET_DVR_GetLastError();
			//m_pHikDevice->m_pRecvResponse(request);
			//LOG_TRACE(g_nLogID, "Exit");
			response_(deviceId_, 1);
			return;
		}

		response_(deviceId_, 0);
	}
	
}


//DataProc::DataProc(std::function<void(DWORD dwDataType, BYTE*pBuffer, DWORD dwBufSize)> dataProc):
//	dataProc_(dataProc)
DataProc::DataProc(ProcFunc dataProc, std::string id) :
	dataProc_(dataProc), deviceId_(id)
{
	
}
