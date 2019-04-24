#pragma once

#include "stream.h"
#include <memory>

class StreamManager;

class StreamI : public Media::Stream
{
public:
	StreamI();
	~StreamI();
	 virtual void openRealStreamAsync(::Media::RealStreamReqParam, ::std::function<void(bool, const ::Media::RealStreamRespParam&)>, ::std::function<void(::std::exception_ptr)>, const ::Ice::Current&);
	 virtual void closeStreamAsync(::std::string, ::std::string, ::std::function<void()>, ::std::function<void(::std::exception_ptr)>, const ::Ice::Current&);
	 virtual void getStreamStatic(::std::string id, ::Media::StreamStatic& stat, const ::Ice::Current&);
	 virtual void getRecordFiles(::std::string, ::std::string, const ::Ice::Current&) {}
	 virtual void openVodStream(::Media::RealStreamReqParam, ::Media::RealStreamRespParam&, const ::Ice::Current&) {}
	 virtual void closeVodStream(::std::string, ::std::string, const ::Ice::Current&) {}
	 virtual void controlVodStream(::std::string, ::std::string, ::std::string, const ::Ice::Current&) {}

private:
	//std::unique_ptr<StreamManager> sm_;
};