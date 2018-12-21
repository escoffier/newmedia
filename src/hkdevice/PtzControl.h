#pragma once
#include "AbstractOperation.h"
#include <Windows.h>
#include <functional>
class PtzControl : public AbstractOperation
{
public:
	PtzControl(std::shared_ptr<Device> device, std::string ptzcmd , std::function<void(std::string, int )> response);
	~PtzControl();

	virtual void Process();
private:
	bool DecodePTZCommand(const std::string& stsipid, std::string sCode, DWORD &dwPTZCommand, DWORD &dwStop, DWORD &dwSpeed);
	bool DecodePTZCruise(std::string scode, DWORD &dwPTZCruiseCmd, BYTE &byCruiseRoute, BYTE &byCruisePoint, WORD &wInput);
	bool DecodePTZPreset(std::string sCode, DWORD& dwPTZPresetCmd, DWORD& dwPresetIndex);

private:
	std::string ptzcmd_;
	std::function<void(std::string, int )> response_;
};

