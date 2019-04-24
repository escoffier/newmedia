#pragma once
#include <functional>
#include "callback.h"
#include "stream.h"
#include "glog/logging.h"

class IceCallBack : public CallBack
{
  public:
    IceCallBack(std::function<void(bool, const ::Media::RealStreamRespParam &)> cb)
    {
        //auto& result = flag_;
        //auto& resp = resp_;
        func = [cb, &resp_ = resp_](bool flag) {
            LOG(INFO) << "IceCallBack resp_.id: " << resp_.id;
            cb(flag, resp_);
        };
    }

    void operator()(const dt::OpenRealStreamResp &resp) override
    {
        resp_.id = resp.id;
        resp_.callid = resp.callid;
        resp_.sourceip = resp.sourceip;
        resp_.sourceport = resp.sourceport;
        LOG(INFO) << "resp_.id: " << resp_.id;
        func(resp.succed);
    }

    Media::RealStreamRespParam resp_;
    bool flag_;
    //std::function<void(bool)> func;
};

class IceCloseCallBack : public CallBack
{
    public:
      IceCloseCallBack(std::function<void()> cb)
      {
          func = cb;
      }

      void operator()() override
      {
          func();
      }

    private:
      std::function<void()> func;
};