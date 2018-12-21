#pragma once

#include <map>
#include <memory>
#include <mutex>
#include "packetsender.h"
#include <queue>

class AVPacket
{
public:
  AVPacket(unsigned int len) : data_(new char[len]), length_(len), pos_(0) {}
  unsigned int availiable() const { return length_ - pos_; }
  ~AVPacket() { delete[] data_; }

  unsigned int size() { return length_; }
  char *data() { return data_; }

private:
  char *data_;
  unsigned int length_;
  unsigned int pos_;
};

class Buffer
{
  using PktSender = std::shared_ptr<PaketSender>;

public:
  Buffer();
  ~Buffer();

  void operator()();
  void processData();
  void addSender(const std::string &callid, std::shared_ptr<PaketSender> sender);
  void removeSender(const std::string &callid);
  void clearSenders();
  int senderNumber();

  std::shared_ptr<AVPacket> popPacket();
  void pushPacket(std::shared_ptr<AVPacket> pkt);

private:
  std::map<std::string, PktSender> senders_;
  std::queue<std::shared_ptr<AVPacket>> packetQueue_;
  std::mutex packetMutex_;
  std::mutex destsmutex_;
};