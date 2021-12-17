#include "AppointmentError.h"
#include <mutex>
#include <map>
#include <string>
#include "util.h"

std::map<int, std::string> mapErr_;
std::once_flag flag;

const int AppointmentError::ErrOK = 200;
const int AppointmentError::ErrVisitRefuse = 0x20304;
const int AppointmentError::ErrNoFace = 0x20404;
const int AppointmentError::ErrVisitNoReceiver = 0x20405;
const int AppointmentError::ErrVisitAddRecord = 0x20406;
const int AppointmentError::ErrGetImg = 0x20407;
const int AppointmentError::ErrVisitProcessed = 0x20408;
const int AppointmentError::ErrVisitOverTime = 0x20409;
const int AppointmentError::ErrNoTarget = 0x2040A;
const int AppointmentError::ErrDelTarget = 0x2040B;
const int AppointmentError::ErrDeviceOffline = 0x2040C;
const int AppointmentError::ErrPhoneExist = 0x2040D;
const int AppointmentError::ErrDelFromDevice = 0x2040E;
const int AppointmentError::ErrHouseHolderExist = 0x2040F;
const int AppointmentError::ErrIdExist = 0x20410;
const int AppointmentError::ErrBindHouse = 0x20411;
const int AppointmentError::ErrVisitNotBelongToVillage = 0x20412;
const int AppointmentError::ErrUnknown = 0xfffff;

static void init()
{
	mapErr_[AppointmentError::ErrOK] = util::gbk_to_utf_8("成功");
	mapErr_[AppointmentError::ErrVisitRefuse] = util::gbk_to_utf_8("访客审核拒绝");
	mapErr_[AppointmentError::ErrNoFace] = util::gbk_to_utf_8("添加人脸库失败");
	mapErr_[AppointmentError::ErrVisitNoReceiver] = util::gbk_to_utf_8("被访人不存在");
	mapErr_[AppointmentError::ErrVisitAddRecord] = util::gbk_to_utf_8("添加人脸访问记录失败");
	mapErr_[AppointmentError::ErrGetImg] = util::gbk_to_utf_8("图片获取失败");
	mapErr_[AppointmentError::ErrVisitProcessed] = util::gbk_to_utf_8("预约记录已处理");
	mapErr_[AppointmentError::ErrVisitOverTime] = util::gbk_to_utf_8("预约已过期");
	mapErr_[AppointmentError::ErrNoTarget] = util::gbk_to_utf_8("目标不存在");
	mapErr_[AppointmentError::ErrDelTarget] = util::gbk_to_utf_8("删除目标失败");
	mapErr_[AppointmentError::ErrDeviceOffline] = util::gbk_to_utf_8("设备不在线");
	mapErr_[AppointmentError::ErrPhoneExist] = util::gbk_to_utf_8("电话号码已经存在");
	mapErr_[AppointmentError::ErrDelFromDevice] = util::gbk_to_utf_8("从设备删除目标失败");
	mapErr_[AppointmentError::ErrHouseHolderExist] = util::gbk_to_utf_8("房屋户主已存在");
	mapErr_[AppointmentError::ErrIdExist] = util::gbk_to_utf_8("证件号重复");
	mapErr_[AppointmentError::ErrBindHouse] = util::gbk_to_utf_8("房屋绑定失败");
	mapErr_[AppointmentError::ErrVisitNotBelongToVillage] = util::gbk_to_utf_8("被访人不属于小区");
	mapErr_[AppointmentError::ErrUnknown] = util::gbk_to_utf_8("未知错误");
}

const char *AppointmentError::getError(int iErrCode)
{
	std::call_once(flag, init);
	static const char *pszBlank = "";
	std::map<int, std::string>::iterator it;
	it = mapErr_.find(iErrCode);
	if (it != mapErr_.end()) {
		return it->second.c_str();
	}else {
		return pszBlank;
	}
}