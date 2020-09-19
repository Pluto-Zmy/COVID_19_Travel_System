#ifndef CONTROL_H
#define CONTROL_H

#include "ProgramLib.h"

// 系统控制类
class Control {
private:
	bool sysPause;								// 系统状态
	std::thread* sysClock;						// 系统主时钟线程
	std::thread* travelStatusRefresher;			// 系统刷新器线程
	void checkProcessExist() const;				// 判断是否已有此程序实例运行
	void startLogSystem();						// 启动日志系统
	void startSysClock();						// 启动系统时钟
	void incSysTime();							// 更新系统时钟
	void initCityList();						// 初始化城市列表
	void initSchedule();						// 初始化时刻表
	void startStatusRefresher();				// 启动旅客状态更新系统
	void updateSystem() const;					// 更新系统状态
public:
	CityList cityList;							// 城市列表
	Schedule schedule;							// 时刻表
	InstanceList instanceList;					// 旅行实例列表
	int instanceNum;							// 旅行实例个数
	int cityNum;								// 城市个数
	Time sysTimeNormal;							// 常规精度系统时钟
	Time sysTimePrecise;						// 高精度系统时钟
	std::ofstream* logFile;						// 日志文件输出流
	std::vector<std::string> vehKindStr;		// 交通工具类型字符
	std::vector<std::string> dangerStr;			// 城市风险类型字符
	Control();									// 系统初始化
	~Control();									// 系统关闭
	bool addTravelInstance(InputData& input_);	// 添加旅行实例
	City* IDtoCity(CityID id_);					// 根据城市编号获得城市指针
	// 获取计划中两相邻交通工具等待时间
	Time getWaitTime(Time lastVehArriveTime_, Time nextVehStartTime_) const;
	void pause();								// 系统暂停
	void proceed();								// 系统继续
	bool isPaused() const;						// 获取系统状态
	void openSupport() const;					// 打开在线支持页面
};

#endif // !CONTROL_H