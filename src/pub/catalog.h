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
	std::string m_id;				// 设备/区域/系统编码（必选）
	std::string m_channel;			// PVG对应的AV通道(PVG通道专用)
	std::string m_name;				// 设备/区域/系统名称（必选）
	std::string m_manufacturer;		// 当为设备时，设备厂商（必选）
	std::string m_model;			// 当为设备时，设备型号（必选）
	std::string m_owner;			// 当为设备时，设备归属（必选）
	std::string m_civil;			// 行政区域（必选）
	std::string m_block;			// 警区（可选）
	std::string m_address;			// 当为设备时，安装地址（必选）
	std::string m_parental;			// 当为设备时，是否有子设备（必选）1有，0没有
	std::string m_parentid;			// 父设备/区域/系统ID（可必选
	std::string m_safetyway;		// 信令安全模式（可选）缺省为0； 0：不采用；2：S/MIME签名方式；3：S/MIME加密签名同时采用方式；4：数字摘要方式	
	std::string m_registerway;		// 注册方式（必选）缺省为1； 1： 符合sip3261标准的认证注册模式；2：基于口令的双向认证注册模式；3： 基于数字证书的双向认证注册模式
	std::string m_certnum;			// 证书序列号（有证书的设备必选）
	std::string m_certifiable;		// 证书有效标识（有证书的设备必选）缺省为0；证书有效标识： 0：无效 1：有效
	std::string m_errcode;			// 无效原因码（有证书且证书无效的设备必选）
	std::string m_endtime;			// 证书终止有效期（有证书的设备必选）
	std::string m_secrecy;			// 保密属性（必选）缺省为0；0：不涉密，1：涉密
	std::string m_ip;				// 设备/区域/系统IP地址（可选）
	std::string m_port;				// 设备/区域/系统端口（可选）
	std::string m_password;			// 设备口令（可选）
	std::string m_status;			// 设备状态（必选）
	std::string m_longitude;		// 经度（可选）
	std::string m_latitude;			// 纬度（可选）
	std::string m_ptz;				// 摄像机类型扩展，标识摄像机类型：1-球机；2-半球；3-固定枪机；4-遥控枪机。当目录项为摄像机时可选。
	std::string m_position;			// 摄像机位置类型扩展。1-省际检查站、2-党政机关、3-车站码头、4-中心广场、5-体育场馆、6-商业中心、7-宗教场所、8-校园周边、9-治安复杂区域、10-交通干线。当目录项为摄像机时可选。
	std::string m_room;				// 摄像机安装位置室外、室内属性。1-室外、2-室内。当目录项为摄像机时可选，缺省为1
	std::string m_use;				// 摄像机用途属性。1-治安、2-交通、3-重点。当目录项为摄像机时可选
	std::string m_supplylight;		// 摄像机补光属性。1-无补光、2-红外补光、3-白光补光。当目录项为摄像机时可选，缺省为1
	std::string m_direction;		// 摄像机监视方位属性。1-东、2-西、3-南、4-北、5-东南、6-东北、7-西南、8-西北。当目录项为摄像机时且为固定摄像机或设置看守位摄像机时可选
	std::string m_resolution;		// 摄像机支持的分辨率，可有多个分辨率值，各个取值见以“/”分隔。分辨率取值参见国标附录F中SDP f字段规定。当目录项为摄像机时可选
	std::string m_businessgroup;	// 虚拟组织所属的业务分组ID，业务分组根据特定的业务需求制定，一个业务分组包含一组特定的虚拟组织
	bool        m_config;
	std::string m_platfromid;       //设备所属的平台id
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