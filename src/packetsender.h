#pragma once
#include <cstdint>
#include <string>
#include <map>

class PaketSender
{
  public:
    PaketSender(const std::string & destIp, int destPort): destip_(destIp),  destport_(destPort)
    {
      
    }
    virtual bool init() = 0;
    virtual void send(uint8_t *data, size_t len) = 0;
    
    void setParam(const std::string &key, const std::string &value)
    {
        paramsmap_.emplace(key, value);
    }

  protected:
    std::map<std::string, std::string> paramsmap_;
    std::string destip_;
    uint32_t destport_;
};