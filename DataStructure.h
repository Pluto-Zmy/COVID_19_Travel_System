#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include "ProgramLib.h"

// 城市类
class City {
private:
	CityID ID;										// 城市编号
	CityName name;									// 城市名称
	CityKind kind;									// 城市风险类型
	QPointF pos;									// 城市坐标
public:
	City(
		CityID id_, CityName name_, CityKind kind_, int posX_, int posY_);
	~City() = default;
	CityID getID() const;							// 获取城市编号
	CityName getName() const;						// 获取城市名称
	CityKind getKind() const;						// 获取城市风险类型
	QPointF getIconPos() const;						// 获取图标绘制位置坐标
	QPointF getLinePos() const;						// 获取路线绘制位置坐标
	// 根据停留时间获取城市危险指数
	DangerIndex getCityDangerIndex(Time stayTime_) const;
};

// 交通工具类
class Vehicle {
private:
	VehID ID;										// 交通工具编号
	VehKind kind;									// 交通工具种类
	VehName name;									// 交通工具名称
	City* srcCity, * dstCity;						// 交通工具始发站和终到站
	Time startTime, runTime;						// 交通工具开点和运行时间
public:
	Vehicle(
		VehID vehID_, VehKind kind_, VehName name_,
		CityID srcID_, CityID dstID_, Time startTime_, Time runTime_);
	~Vehicle() = default;
	VehID getID() const;							// 获取交通工具编号
	VehKind getKind() const;						// 获取交通工具种类
	VehName getName() const;						// 获取交通工具名称
	City* const getSrcCity() const;					// 获取交通工具始发站
	City* const getDstCity() const;					// 获取交通工具终到站
	Time getStartTime() const;						// 获取交通工具开点
	Time getRunTime() const;						// 获取交通工具运行时间
	Time getArriveTime() const;						// 获取交通工具到达时间
	DangerIndex getVehDangerIndex() const;			// 获取交通工具危险指数
	std::string toUIString() const;					// 交通工具描述
};

// 旅行实例类
class TravelInstance {
private:
	TravelID ID;									// 旅客编号
	PlanKind kind;									// 旅行实例类型
	City* beginCity, * endCity;						// 旅行起点和旅行终点
	Time limitTime, beginTime, lastTime;			// 限时、开始时间和持续时间
	Plan plan;										// 旅行方案
	DangerIndex dangerIndex;						// 旅行总风险值
	std::vector<Time> keyTime;						// 旅行关键时间节点序列
public:
	TravelInstance(
		TravelID id_, PlanKind kind_, CityID beginID_, CityID endID_,
		Time beginTime_, Time lastTime_, DangerIndex dangerIndex_,
		Plan* plan_, Time limitTime_ = INS_TIME_MAX);
	~TravelInstance() = default;
	TravelID getID() const;							// 获取旅客编号
	City* const getBeginCity() const;				// 获取旅行起点
	City* const getEndCity() const;					// 获取旅行终点
	Time getBeginTime() const;						// 获取旅行开始时间
	Time getLastTime() const;						// 获取旅行持续时间
	const Plan* const getPlan() const;				// 获取旅行计划
	// 获取关键时间节点序列
	const std::vector<Time>* const getKeyTime() const;
	DangerIndex getDangerIndex() const;				// 获取旅行总风险值
	std::string getStatusUI(Time curTime_) const;	// 旅行状态 UI 描述
	std::string getStatusLog(Time curTime_) const;	// 旅行状态日志描述
	std::string toStringLog() const;				// 旅行信息日志描述
};

// 输入信息类
class InputData {
public:
	CityID beginID, endID;							// 起点和终点城市编号
	Request request;								// 旅行请求类型
	Time beginTime;									// 开始时间
	Time limitTime;									// 旅行限时
	InputData(
		CityID beginID_, CityID endID_, Time beginTime_,
		Request request_, Time limitTime_ = INS_TIME_MAX);
};

#endif // !DATA_STRUCTURE_H