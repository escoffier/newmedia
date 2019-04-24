#pragma once
#include <memory>
#include <string>
#include <map>

class Camara
{
public:
	Camara();
	~Camara();

	friend class CamaraManager;
private:
	std::string id_;
	std::string ip_;
	std::string port_;
	std::string name_;
	std::string pwd_;
};

class CamaraManager
{
	CamaraManager();
public:
	~CamaraManager();
	static CamaraManager * getInstance();
	void insert(std::shared_ptr<Camara> camara);
	std::shared_ptr<Camara> find(std::string id);

private:
	static CamaraManager *instance_;

	std::map<std::string, std::shared_ptr<Camara>> camaraMap_;

	//std::map
};
