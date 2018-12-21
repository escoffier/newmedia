#include "camara.h"

Camara::Camara():
id_(), ip_(), port_(), name_(), pwd_()
{
}

Camara::~Camara()
{
}

CamaraManager::~CamaraManager()
{
}

CamaraManager * CamaraManager::instance_ = nullptr;

CamaraManager * CamaraManager::getInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new CamaraManager;
	}
	return instance_;
}

CamaraManager::CamaraManager()
{

}

void CamaraManager::insert(std::shared_ptr<Camara> camara)
{
	camaraMap_.emplace(camara->id_, camara);
}

std::shared_ptr<Camara> CamaraManager::find(std::string id)
{
	auto search = camaraMap_.find(id);
	if (search != camaraMap_.end())
	{
		return search->second;
	}
	else
	{
		return std::shared_ptr<Camara>();
	}
}
