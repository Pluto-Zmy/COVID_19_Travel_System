#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include "ProgramLib.h"

// ������
class City {
private:
	CityID ID;										// ���б��
	CityName name;									// ��������
	CityKind kind;									// ���з�������
	QPointF pos;									// ��������
public:
	City(
		CityID id_, CityName name_, CityKind kind_, int posX_, int posY_);
	~City() = default;
	CityID getID() const;							// ��ȡ���б��
	CityName getName() const;						// ��ȡ��������
	CityKind getKind() const;						// ��ȡ���з�������
	QPointF getIconPos() const;						// ��ȡͼ�����λ������
	QPointF getLinePos() const;						// ��ȡ·�߻���λ������
	// ����ͣ��ʱ���ȡ����Σ��ָ��
	DangerIndex getCityDangerIndex(Time stayTime_) const;
};

// ��ͨ������
class Vehicle {
private:
	VehID ID;										// ��ͨ���߱��
	VehKind kind;									// ��ͨ��������
	VehName name;									// ��ͨ��������
	City* srcCity, * dstCity;						// ��ͨ����ʼ��վ���յ�վ
	Time startTime, runTime;						// ��ͨ���߿��������ʱ��
public:
	Vehicle(
		VehID vehID_, VehKind kind_, VehName name_,
		CityID srcID_, CityID dstID_, Time startTime_, Time runTime_);
	~Vehicle() = default;
	VehID getID() const;							// ��ȡ��ͨ���߱��
	VehKind getKind() const;						// ��ȡ��ͨ��������
	VehName getName() const;						// ��ȡ��ͨ��������
	City* const getSrcCity() const;					// ��ȡ��ͨ����ʼ��վ
	City* const getDstCity() const;					// ��ȡ��ͨ�����յ�վ
	Time getStartTime() const;						// ��ȡ��ͨ���߿���
	Time getRunTime() const;						// ��ȡ��ͨ��������ʱ��
	Time getArriveTime() const;						// ��ȡ��ͨ���ߵ���ʱ��
	DangerIndex getVehDangerIndex() const;			// ��ȡ��ͨ����Σ��ָ��
	std::string toUIString() const;					// ��ͨ��������
};

// ����ʵ����
class TravelInstance {
private:
	TravelID ID;									// �ÿͱ��
	PlanKind kind;									// ����ʵ������
	City* beginCity, * endCity;						// �������������յ�
	Time limitTime, beginTime, lastTime;			// ��ʱ����ʼʱ��ͳ���ʱ��
	Plan plan;										// ���з���
	DangerIndex dangerIndex;						// �����ܷ���ֵ
	std::vector<Time> keyTime;						// ���йؼ�ʱ��ڵ�����
public:
	TravelInstance(
		TravelID id_, PlanKind kind_, CityID beginID_, CityID endID_,
		Time beginTime_, Time lastTime_, DangerIndex dangerIndex_,
		Plan* plan_, Time limitTime_ = INS_TIME_MAX);
	~TravelInstance() = default;
	TravelID getID() const;							// ��ȡ�ÿͱ��
	City* const getBeginCity() const;				// ��ȡ�������
	City* const getEndCity() const;					// ��ȡ�����յ�
	Time getBeginTime() const;						// ��ȡ���п�ʼʱ��
	Time getLastTime() const;						// ��ȡ���г���ʱ��
	const Plan* const getPlan() const;				// ��ȡ���мƻ�
	// ��ȡ�ؼ�ʱ��ڵ�����
	const std::vector<Time>* const getKeyTime() const;
	DangerIndex getDangerIndex() const;				// ��ȡ�����ܷ���ֵ
	std::string getStatusUI(Time curTime_) const;	// ����״̬ UI ����
	std::string getStatusLog(Time curTime_) const;	// ����״̬��־����
	std::string toStringLog() const;				// ������Ϣ��־����
};

// ������Ϣ��
class InputData {
public:
	CityID beginID, endID;							// �����յ���б��
	Request request;								// ������������
	Time beginTime;									// ��ʼʱ��
	Time limitTime;									// ������ʱ
	InputData(
		CityID beginID_, CityID endID_, Time beginTime_,
		Request request_, Time limitTime_ = INS_TIME_MAX);
};

#endif // !DATA_STRUCTURE_H