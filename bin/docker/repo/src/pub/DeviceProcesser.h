#pragma once
//#include "WorkorQueue.h"
//#include "AbstractOperation.h"
//#include "Catalog.h"
//#include "ControlParam.h"
//#include "utility.h"
#include <functional>

#if defined(_MSC_VER)
//  Microsoft 
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
//  GCC
#define EXPORT __attribute__((visibility("default")))
#define IMPORT
#else
//  do nothing and hope for the best?
#define EXPORT
#define IMPORT
#pragma warning Unknown dynamic link import/export semantics.
#endif


namespace dt
{
	struct DeviceStatus
	{
		std::string id;
		int status;
	};
	
};

class CamaraController
{
public:
	CamaraController(){}
	virtual ~CamaraController() {}

	//virtual bool Login(std::shared_ptr<CCatalog> ctg) { return false; };
	virtual bool Login(std::string id, std::string ip, int port, std::string user, std::string pwd) { return false; };

	virtual bool isLogin() const { return false; } 

	virtual void ptzControl(std::string id, std::string cmd) {  };

	virtual void getDeviceStatus(std::string id, dt::DeviceStatus &st) {};

	//virtual bool openRealStream(const dt::OpenRealStream& param) = 0;
	virtual bool openRealStream(std::string id, std::string ip, int port, std::string user,
		std::string pwd, std::function<void(char *, uint32_t)> datacb_) = 0;

	virtual bool closeRealStream(const std::string& id) = 0;
protected:
	//ThreadsafeQueue queue_;

};

