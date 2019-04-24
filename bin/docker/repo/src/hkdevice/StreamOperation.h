#pragma once
#include "AbstractOperation.h"
#include <Windows.h>
#include <functional>
#include <atomic>
class StreamOperation : public AbstractOperation
{
public:
	typedef std::function<void(/*std::shared_ptr<Device> device,*/ std::string id, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize)> ProcFunc;
	//StreamOperation(std::shared_ptr<Device> device, std::function<void(std::string, int)> response, std::function<void(/*std::shared_ptr<Device> device,*/std::string id, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize)>);
	~StreamOperation();
	virtual void Process();
private:
	std::function<void(std::string, int)> response_;
public:
	//std::function<void(/*std::shared_ptr<Device> device,*/ DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize)> dataProc_;
	ProcFunc dataProc_;
};

class StopStream
{
public:
	StopStream(std::string id, std::function<void(std::string, int)>);
	~StopStream();

	virtual void Process();
private:
	std::function<void(std::string, int)> response_;
private:
	std::string deviceId_;

};

class DataProc
{
public:
	typedef std::function<void(/*std::shared_ptr<Device> device,*/std::string id, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize)> ProcFunc;

	//DataProc(std::function<void(/*std::shared_ptr<Device> device,*/ DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize)> dataProc);
	DataProc(ProcFunc dataProc, std::string id);
	void operator() (std::string id, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize) { dataProc_(id, dwDataType, pBuffer, dwBufSize); };
public:
	//std::function<void(/*std::shared_ptr<Device> device,*/ DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize)> dataProc_;
	ProcFunc dataProc_;
	std::string deviceId_;
	//std::atomic_int usedCount;
};