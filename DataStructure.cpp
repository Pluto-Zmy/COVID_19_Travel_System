#include "Control.h"
#include "DataStructure.h"

extern Control System;

City::City(
	CityID id_, CityName name_, CityKind kind_, int posX_, int posY_)
	: ID(id_), name(name_), kind(kind_), pos(posX_, posY_) {
}

CityID City::getID() const {
	return ID;
}

CityName City::getName() const {
	return name;
}

CityKind City::getKind() const {
	return kind;
}

QPointF City::getIconPos() const {
	return pos - QPointF(24, 24);
}

QPointF City::getLinePos() const {
	return pos;
}

// 城市停留风险值 = 参数(停留时间) * 单位时间风险值
DangerIndex City::getCityDangerIndex(Time stayTime_) const {
	switch (kind) {
		case LOW:	return stayTime_ * 2;
		case MID:	return stayTime_ * 5;
		case HIGH:	return stayTime_ * 9;
		default:	return 0;
	}
}

Vehicle::Vehicle(
	VehID vehID_, VehKind kind_, VehName name_,
	CityID srcID_, CityID dstID_, Time startTime_, Time runTime_)
	: ID(vehID_), kind(kind_), name(name_),
	srcCity(nullptr), dstCity(nullptr), startTime(startTime_), runTime(runTime_) {
	// 根据城市编号获取城市指针
	srcCity = System.IDtoCity(srcID_);
	dstCity = System.IDtoCity(dstID_);
}

VehID Vehicle::getID() const {
	return ID;
}

VehKind Vehicle::getKind() const {
	return kind;
}

VehName Vehicle::getName() const {
	return name;
}

City* const Vehicle::getSrcCity() const {
	return srcCity;
}

City* const Vehicle::getDstCity() const {
	return dstCity;
}

Time Vehicle::getStartTime() const {
	return startTime;
}

Time Vehicle::getRunTime() const {
	return runTime;
}

Time Vehicle::getArriveTime() const {
	return (startTime + runTime) % 24;
}

// 交通工具风险值 = 始发城市单位时间风险值 * 交通工具单位时间风险值 * 运行时间
DangerIndex Vehicle::getVehDangerIndex() const {
	switch (kind) {
		case AIRPLANE:	return srcCity->getCityDangerIndex(1) * 9 * getRunTime();
		case TRAIN:		return srcCity->getCityDangerIndex(1) * 5 * getRunTime();
		case BUS:		return srcCity->getCityDangerIndex(1) * 2 * getRunTime();
		default:		return 0;
	}
}

// 交通工具描述信息，用于计划展示
std::string Vehicle::toUIString() const {
	std::string infoUIStr;
	infoUIStr = "在【" + srcCity->getName() + "市】" +
		"于 " + std::to_string(startTime) + ":00" +
		"\t乘坐 " + name + " 次" + System.vehKindStr[kind] +
		"\t历时 " + std::to_string(runTime) + " 小时" +
		"\t于 " + std::to_string(getArriveTime()) + ":00" +
		" 到达【" + dstCity->getName() + "市】";
	return infoUIStr;
}

TravelInstance::TravelInstance(
	TravelID id_, PlanKind kind_, CityID beginID_, CityID endID_,
	Time beginTime_, Time lastTime_, DangerIndex dangerIndex_,
	Plan* plan_, Time limitTime_)
	: ID(id_), kind(kind_), beginCity(nullptr), endCity(nullptr),
	beginTime(beginTime_), lastTime(lastTime_), dangerIndex(dangerIndex_),
	plan(*plan_), limitTime(limitTime_) {
	beginCity = System.IDtoCity(beginID_);
	endCity = System.IDtoCity(endID_);
	// 根据生成的旅行方案获取关键时间节点序列
	for (auto vehIter = plan.begin(); vehIter != plan.end(); vehIter++) {
		// 定义每班车次的到达时间
		Time originTime;
		// 如果在始发地，特定义为旅行开始时间；否则为实际到达时间
		if ((*vehIter)->getSrcCity() == beginCity) {
			originTime = beginTime % 24;
		}
		else {
			originTime = (*(vehIter - 1))->getArriveTime();
		}
		// 按时间次序存储候车时间与乘车时间
		keyTime.push_back(System.getWaitTime(originTime, (*vehIter)->getStartTime()));
		keyTime.push_back((*vehIter)->getRunTime());
	}
	// 累加，变成方案内每个关键时间节点的时刻
	for (auto timeIter = keyTime.begin() + 1; timeIter != keyTime.end(); timeIter++) {
		*timeIter += *(timeIter - 1);
	}
	// 在累加的基础上再分别加上旅行起始时间，以生成全局时间序列
	for (auto& time : keyTime) {
		time += beginTime;
	}
}

TravelID TravelInstance::getID() const {
	return ID;
}

City* const TravelInstance::getBeginCity() const {
	return beginCity;
}

City* const TravelInstance::getEndCity() const {
	return endCity;
}

Time TravelInstance::getBeginTime() const {
	return beginTime;
}

Time TravelInstance::getLastTime() const {
	return lastTime;
}

const Plan* const TravelInstance::getPlan() const {
	return &plan;
}

const std::vector<Time>* const TravelInstance::getKeyTime() const {
	return &keyTime;
}

DangerIndex TravelInstance::getDangerIndex() const {
	return dangerIndex;
}

// 根据系统当前时间确定旅客状态
std::string TravelInstance::getStatusUI(Time curTime_) const {
	std::string statusStr;
	// 如果当前时间小于旅行开始时间，说明该旅行自定义了开始时间，且还未开始
	if (curTime_ < beginTime) {
		statusStr = "\n" + beginCity->getName() + "（尚未开始）\n";
	}
	// 如果当前时间大于旅行结束时间，说明已到达目的地
	else if (curTime_ >= *(keyTime.end() - 1)) {
		statusStr = "\n" + endCity->getName() + "（终点）\n";
	}
	// 处于旅行中
	else {
		// 遍历关键时间节点序列
		for (int i{0}; i < keyTime.size(); i++) {
			// 寻找系统当前时间落在那个区间内
			if (curTime_ < *(keyTime.begin() + i)) {
				Vehicle* curVeh = *(plan.begin() + i / 2);
				// 如果落在偶数区间，说明在候车；否则在乘车
				if (0 == i % 2) {
					statusStr = curVeh->getSrcCity()->getName() + "（候车）\n等候前往【" +
						curVeh->getDstCity()->getName() + "市】的 " +
						curVeh->getName() + " 次" + System.vehKindStr[curVeh->getKind()] + "\n" +
						"还需等待 " + std::to_string(System.getWaitTime(System.sysTimeNormal % 24, curVeh->getStartTime())) + " 小时";
				}
				else {
					statusStr = curVeh->getName() + " 次" + System.vehKindStr[curVeh->getKind()] +
						"\n" + curVeh->getSrcCity()->getName() + " → " + curVeh->getDstCity()->getName() + "\n" +
						"还需乘坐 " + std::to_string(max((*(keyTime.begin() + i - 1) + curVeh->getRunTime() - System.sysTimeNormal), 0)) + " 小时";
				}
				break;
			}
		}
	}
	return statusStr;
}

// 原理同上，只是用于日志输出，格式略有不同
std::string TravelInstance::getStatusLog(Time curTime_) const {
	std::string statusStr;
	if (curTime_ < beginTime) {
		statusStr = beginCity->getName() + "（尚未开始）";
	}
	else if (curTime_ >= *(keyTime.end() - 1)) {
		statusStr = endCity->getName() + "（终点）";
	}
	else {
		for (int i{0}; i < keyTime.size(); i++) {
			if (curTime_ < *(keyTime.begin() + i)) {
				Vehicle* curVeh = *(plan.begin() + i / 2);
				if (0 == i % 2) {
					statusStr = curVeh->getSrcCity()->getName() + "(候车)" + ", 还需等待 " +
						std::to_string(System.getWaitTime(System.sysTimeNormal % 24, curVeh->getStartTime())) + " 小时";
				}
				else {
					statusStr = curVeh->getName() + " 次" + System.vehKindStr[curVeh->getKind()] +
						"[" + curVeh->getSrcCity()->getName() + "->" + curVeh->getDstCity()->getName() + "]" +
						", 还需乘坐 " + std::to_string(max((*(keyTime.begin() + i - 1) + curVeh->getRunTime() - System.sysTimeNormal), 0)) + " 小时";
				}
				break;
			}
		}
	}
	return statusStr;
}

std::string TravelInstance::toStringLog() const {
	std::string infoStr;
	if (NOLIMIT == kind) {
		infoStr = "ID: " + std::to_string(ID) + ", 类型: 不限时" +
			", 起点: " + beginCity->getName() + ", 终点: " + endCity->getName();
	}
	else {
		infoStr = "ID: " + std::to_string(ID) + ", 类型: 限时" +
			", 起点: " + beginCity->getName() + ", 终点: " + endCity->getName() +
			", 限时: " + std::to_string(limitTime) + " 小时";
	}
	return infoStr;
}

InputData::InputData(
	CityID beginID_, CityID endID_, Time beginTime_,
	Request request_, Time limitTime_)
	: beginID(beginID_), endID(endID_), beginTime(beginTime_),
	request(request_), limitTime(limitTime_) {
}