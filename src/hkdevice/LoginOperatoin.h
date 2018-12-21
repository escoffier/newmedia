#pragma once
#include "AbstractOperation.h"
//#include "DeviceProcesser.h"
#include <memory>
class Device;
class LoginOperatoin : public AbstractOperation
{
public:
	//LoginOperatoin(std::shared_ptr<Device> device, std::shared_ptr<DeviceProcesser>);
	LoginOperatoin(std::shared_ptr<Device> device);
	~LoginOperatoin();
	virtual void Process();
private:
	bool needInitSdk;
	//std::shared_ptr<Device> device_;
	//std::shared_ptr<DeviceProcesser> processer_;
};