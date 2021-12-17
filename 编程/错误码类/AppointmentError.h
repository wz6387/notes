#ifndef _APPOINTMENT_ERROR_H_
#define _APPOINTMENT_ERROR_H_


class AppointmentError
{
public:
	static const int ErrOK;
	static const int ErrVisitRefuse;                      // 访客审核拒绝
	static const int ErrNoFace;                           // 添加人脸库失败
	static const int ErrVisitNoReceiver;                  // 被访人不存在
	static const int ErrVisitAddRecord;                   // 添加人脸访问记录失败
	static const int ErrGetImg;                           // 图片获取失败
	static const int ErrVisitProcessed;                   // 预约记录已处理
	static const int ErrVisitOverTime;                    // 预约已过期
	static const int ErrNoTarget;                         // 目标不存在
	static const int ErrDelTarget;                        // 删除目标失败
	static const int ErrDeviceOffline;                    // 设备不在线
	static const int ErrPhoneExist;                       // 电话号码已经存在
	static const int ErrDelFromDevice;                    // 从设备删除目标失败
	static const int ErrHouseHolderExist;                 // 房屋户主已存在
	static const int ErrIdExist;                          // 证件号重复
	static const int ErrBindHouse;                        // 房屋绑定失败
	static const int ErrVisitNotBelongToVillage;          // 被访人不属于小区
	static const int ErrUnknown;                          // 未知错误


	static const char *getError(int iErrCode);
};

#endif
