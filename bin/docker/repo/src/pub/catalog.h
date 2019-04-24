#pragma once
#include <string>
#include <map>
#include <memory>

class CCatalog
{
public:

	CCatalog();
	~CCatalog();

public:
	bool operator!=(const CCatalog& catalog) const;
	bool operator==(const CCatalog& catalog) const;

public:
	const std::string& GetID() const;
	void SetID(const std::string &id);
	void SetID(std::string &&id);

	const std::string& GetChannel() const;
	void SetChannel(const std::string& channel);

	const std::string& GetName() const;
	void SetName(const std::string &name);
	void SetName( std::string &&name);

	const std::string& GetManufacturer() const;
	void SetManufacturer(const std::string &manufacturer);

	const std::string& GetModel() const;
	void SetModel(const std::string &model);

	const std::string& GetOwner() const;
	void SetOwner(const std::string &owner);

	const std::string& GetCivil() const;
	void SetCivil(const std::string &civil);

	const std::string& GetBlock() const;
	void SetBlock(const std::string &block);

	const std::string& GetAddress() const;
	void SetAddress(const std::string &address);

	const std::string& GetSafetyway() const;
	void SetSafetyway(const std::string &safetyway);

	const std::string& GetRegisterway() const;
	void SetRegisterway(const std::string &registerway);

	const std::string& GetCertnum() const;
	void SetCertnum(const std::string &certnum);

	const std::string& GetCertifiable() const;
	void SetCertifiable(const std::string &certifiable);

	const std::string& GetErrcode() const;
	void SetErrcode(const std::string &errcode);

	const std::string& GetSecrecy() const;
	void SetSecrecy(const std::string &secrecy);

	const std::string& GetParental() const;
	void SetParental(const std::string &parental);

	const std::string& GetParentID() const;
	void SetParentID(const std::string &parentid);

	const std::string& GetEndtime() const;
	void SetEndtime(const std::string &endtime);

	const std::string& GetIp() const;
	void SetIp(const std::string &ip);
	void SetIp( std::string &&ip);

	const std::string& GetPort() const;
	void Setport(const std::string &port);

	const std::string& GetPassword() const;
	void SetPassword(const std::string &password);
	void SetPassword( std::string &&password);

	const std::string& GetStatus() const;
	void SetStatus(const std::string &status);
	void SetOnStatus();
	void SetOffStatus();
	void SetVlostStatus();
	void SetDefectStatus();

	const std::string& GetLongitude() const;
	void SetLongitude(const std::string &longitude);

	const std::string& GetLatitude() const;
	void SetLatitude(const std::string &latitude);

	const std::string& GetPtz() const;
	void SetPtz(const std::string &ptz);

	const std::string& GetPosition() const;
	void SetPosition(const std::string &position);

	const std::string& GetRoom() const;
	void SetRoom(const std::string &room);

	const std::string& GetUse() const;
	void SetUse(const std::string &use);

	const std::string& GetSupplylight() const;
	void SetSupplylight(const std::string &supplylight);

	const std::string& GetDirection() const;
	void SetDirection(const std::string &direction);

	const std::string& GetResolution() const;
	void SetResolution(const std::string &resolution);

	const std::string& GetBusinessgroup() const;
	void SetBusinessgroup(const std::string &businessgroup);

	const std::string& GetPlatformID() const;
	void SetPlatformID(const std::string &platformid);

	const std::string& GetSdkName() const;
	void SetSdkName(const std::string &sdkname);
private:
	std::string m_id;				// �豸/����/ϵͳ���루��ѡ��
	std::string m_channel;			// PVG��Ӧ��AVͨ��(PVGͨ��ר��)
	std::string m_name;				// �豸/����/ϵͳ���ƣ���ѡ��
	std::string m_manufacturer;		// ��Ϊ�豸ʱ���豸���̣���ѡ��
	std::string m_model;			// ��Ϊ�豸ʱ���豸�ͺţ���ѡ��
	std::string m_owner;			// ��Ϊ�豸ʱ���豸��������ѡ��
	std::string m_civil;			// �������򣨱�ѡ��
	std::string m_block;			// ��������ѡ��
	std::string m_address;			// ��Ϊ�豸ʱ����װ��ַ����ѡ��
	std::string m_parental;			// ��Ϊ�豸ʱ���Ƿ������豸����ѡ��1�У�0û��
	std::string m_parentid;			// ���豸/����/ϵͳID���ɱ�ѡ
	std::string m_safetyway;		// ���ȫģʽ����ѡ��ȱʡΪ0�� 0�������ã�2��S/MIMEǩ����ʽ��3��S/MIME����ǩ��ͬʱ���÷�ʽ��4������ժҪ��ʽ	
	std::string m_registerway;		// ע�᷽ʽ����ѡ��ȱʡΪ1�� 1�� ����sip3261��׼����֤ע��ģʽ��2�����ڿ����˫����֤ע��ģʽ��3�� ��������֤���˫����֤ע��ģʽ
	std::string m_certnum;			// ֤�����кţ���֤����豸��ѡ��
	std::string m_certifiable;		// ֤����Ч��ʶ����֤����豸��ѡ��ȱʡΪ0��֤����Ч��ʶ�� 0����Ч 1����Ч
	std::string m_errcode;			// ��Чԭ���루��֤����֤����Ч���豸��ѡ��
	std::string m_endtime;			// ֤����ֹ��Ч�ڣ���֤����豸��ѡ��
	std::string m_secrecy;			// �������ԣ���ѡ��ȱʡΪ0��0�������ܣ�1������
	std::string m_ip;				// �豸/����/ϵͳIP��ַ����ѡ��
	std::string m_port;				// �豸/����/ϵͳ�˿ڣ���ѡ��
	std::string m_password;			// �豸�����ѡ��
	std::string m_status;			// �豸״̬����ѡ��
	std::string m_longitude;		// ���ȣ���ѡ��
	std::string m_latitude;			// γ�ȣ���ѡ��
	std::string m_ptz;				// �����������չ����ʶ��������ͣ�1-�����2-����3-�̶�ǹ����4-ң��ǹ������Ŀ¼��Ϊ�����ʱ��ѡ��
	std::string m_position;			// �����λ��������չ��1-ʡ�ʼ��վ��2-�������ء�3-��վ��ͷ��4-���Ĺ㳡��5-�������ݡ�6-��ҵ���ġ�7-�ڽ̳�����8-У԰�ܱߡ�9-�ΰ���������10-��ͨ���ߡ���Ŀ¼��Ϊ�����ʱ��ѡ��
	std::string m_room;				// �������װλ�����⡢�������ԡ�1-���⡢2-���ڡ���Ŀ¼��Ϊ�����ʱ��ѡ��ȱʡΪ1
	std::string m_use;				// �������;���ԡ�1-�ΰ���2-��ͨ��3-�ص㡣��Ŀ¼��Ϊ�����ʱ��ѡ
	std::string m_supplylight;		// ������������ԡ�1-�޲��⡢2-���ⲹ�⡢3-�׹ⲹ�⡣��Ŀ¼��Ϊ�����ʱ��ѡ��ȱʡΪ1
	std::string m_direction;		// ��������ӷ�λ���ԡ�1-����2-����3-�ϡ�4-����5-���ϡ�6-������7-���ϡ�8-��������Ŀ¼��Ϊ�����ʱ��Ϊ�̶�����������ÿ���λ�����ʱ��ѡ
	std::string m_resolution;		// �����֧�ֵķֱ��ʣ����ж���ֱ���ֵ������ȡֵ���ԡ�/���ָ����ֱ���ȡֵ�μ����긽¼F��SDP f�ֶι涨����Ŀ¼��Ϊ�����ʱ��ѡ
	std::string m_businessgroup;	// ������֯������ҵ�����ID��ҵ���������ض���ҵ�������ƶ���һ��ҵ��������һ���ض���������֯
	bool        m_config;
	std::string m_platfromid;       //�豸������ƽ̨id
	std::string m_sdkname_;
};

// class CatalogManager
// {
	// CatalogManager();
// public:
	// ~CatalogManager();
	// static CatalogManager * getInstance();
	// void insert(std::shared_ptr<CCatalog> ctg);
	// std::shared_ptr<CCatalog> find(std::string id);

// private:
	// static CatalogManager *instance_;

	// std::map<std::string, std::shared_ptr<CCatalog>> catalogmap_;

	//std::map
// };