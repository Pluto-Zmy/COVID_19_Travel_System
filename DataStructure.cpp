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

// ����ͣ������ֵ = ����(ͣ��ʱ��) * ��λʱ�����ֵ
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
	// ���ݳ��б�Ż�ȡ����ָ��
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

// ��ͨ���߷���ֵ = ʼ�����е�λʱ�����ֵ * ��ͨ���ߵ�λʱ�����ֵ * ����ʱ��
DangerIndex Vehicle::getVehDangerIndex() const {
	switch (kind) {
		case AIRPLANE:	return srcCity->getCityDangerIndex(1) * 9 * getRunTime();
		case TRAIN:		return srcCity->getCityDangerIndex(1) * 5 * getRunTime();
		case BUS:		return srcCity->getCityDangerIndex(1) * 2 * getRunTime();
		default:		return 0;
	}
}

// ��ͨ����������Ϣ�����ڼƻ�չʾ
std::string Vehicle::toUIString() const {
	std::string infoUIStr;
	infoUIStr = "�ڡ�" + srcCity->getName() + "�С�" +
		"�� " + std::to_string(startTime) + ":00" +
		"\t���� " + name + " ��" + System.vehKindStr[kind] +
		"\t��ʱ " + std::to_string(runTime) + " Сʱ" +
		"\t�� " + std::to_string(getArriveTime()) + ":00" +
		" ���" + dstCity->getName() + "�С�";
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
	// �������ɵ����з�����ȡ�ؼ�ʱ��ڵ�����
	for (auto vehIter = plan.begin(); vehIter != plan.end(); vehIter++) {
		// ����ÿ�೵�εĵ���ʱ��
		Time originTime;
		// �����ʼ���أ��ض���Ϊ���п�ʼʱ�䣻����Ϊʵ�ʵ���ʱ��
		if ((*vehIter)->getSrcCity() == beginCity) {
			originTime = beginTime % 24;
		}
		else {
			originTime = (*(vehIter - 1))->getArriveTime();
		}
		// ��ʱ�����洢��ʱ����˳�ʱ��
		keyTime.push_back(System.getWaitTime(originTime, (*vehIter)->getStartTime()));
		keyTime.push_back((*vehIter)->getRunTime());
	}
	// �ۼӣ���ɷ�����ÿ���ؼ�ʱ��ڵ��ʱ��
	for (auto timeIter = keyTime.begin() + 1; timeIter != keyTime.end(); timeIter++) {
		*timeIter += *(timeIter - 1);
	}
	// ���ۼӵĻ������ٷֱ����������ʼʱ�䣬������ȫ��ʱ������
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

// ����ϵͳ��ǰʱ��ȷ���ÿ�״̬
std::string TravelInstance::getStatusUI(Time curTime_) const {
	std::string statusStr;
	// �����ǰʱ��С�����п�ʼʱ�䣬˵���������Զ����˿�ʼʱ�䣬�һ�δ��ʼ
	if (curTime_ < beginTime) {
		statusStr = "\n" + beginCity->getName() + "����δ��ʼ��\n";
	}
	// �����ǰʱ��������н���ʱ�䣬˵���ѵ���Ŀ�ĵ�
	else if (curTime_ >= *(keyTime.end() - 1)) {
		statusStr = "\n" + endCity->getName() + "���յ㣩\n";
	}
	// ����������
	else {
		// �����ؼ�ʱ��ڵ�����
		for (int i{0}; i < keyTime.size(); i++) {
			// Ѱ��ϵͳ��ǰʱ�������Ǹ�������
			if (curTime_ < *(keyTime.begin() + i)) {
				Vehicle* curVeh = *(plan.begin() + i / 2);
				// �������ż�����䣬˵���ں򳵣������ڳ˳�
				if (0 == i % 2) {
					statusStr = curVeh->getSrcCity()->getName() + "���򳵣�\n�Ⱥ�ǰ����" +
						curVeh->getDstCity()->getName() + "�С��� " +
						curVeh->getName() + " ��" + System.vehKindStr[curVeh->getKind()] + "\n" +
						"����ȴ� " + std::to_string(System.getWaitTime(System.sysTimeNormal % 24, curVeh->getStartTime())) + " Сʱ";
				}
				else {
					statusStr = curVeh->getName() + " ��" + System.vehKindStr[curVeh->getKind()] +
						"\n" + curVeh->getSrcCity()->getName() + " �� " + curVeh->getDstCity()->getName() + "\n" +
						"������� " + std::to_string(max((*(keyTime.begin() + i - 1) + curVeh->getRunTime() - System.sysTimeNormal), 0)) + " Сʱ";
				}
				break;
			}
		}
	}
	return statusStr;
}

// ԭ��ͬ�ϣ�ֻ��������־�������ʽ���в�ͬ
std::string TravelInstance::getStatusLog(Time curTime_) const {
	std::string statusStr;
	if (curTime_ < beginTime) {
		statusStr = beginCity->getName() + "����δ��ʼ��";
	}
	else if (curTime_ >= *(keyTime.end() - 1)) {
		statusStr = endCity->getName() + "���յ㣩";
	}
	else {
		for (int i{0}; i < keyTime.size(); i++) {
			if (curTime_ < *(keyTime.begin() + i)) {
				Vehicle* curVeh = *(plan.begin() + i / 2);
				if (0 == i % 2) {
					statusStr = curVeh->getSrcCity()->getName() + "(��)" + ", ����ȴ� " +
						std::to_string(System.getWaitTime(System.sysTimeNormal % 24, curVeh->getStartTime())) + " Сʱ";
				}
				else {
					statusStr = curVeh->getName() + " ��" + System.vehKindStr[curVeh->getKind()] +
						"[" + curVeh->getSrcCity()->getName() + "->" + curVeh->getDstCity()->getName() + "]" +
						", ������� " + std::to_string(max((*(keyTime.begin() + i - 1) + curVeh->getRunTime() - System.sysTimeNormal), 0)) + " Сʱ";
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
		infoStr = "ID: " + std::to_string(ID) + ", ����: ����ʱ" +
			", ���: " + beginCity->getName() + ", �յ�: " + endCity->getName();
	}
	else {
		infoStr = "ID: " + std::to_string(ID) + ", ����: ��ʱ" +
			", ���: " + beginCity->getName() + ", �յ�: " + endCity->getName() +
			", ��ʱ: " + std::to_string(limitTime) + " Сʱ";
	}
	return infoStr;
}

InputData::InputData(
	CityID beginID_, CityID endID_, Time beginTime_,
	Request request_, Time limitTime_)
	: beginID(beginID_), endID(endID_), beginTime(beginTime_),
	request(request_), limitTime(limitTime_) {
}