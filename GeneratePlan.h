#ifndef GENERATE_PLAN_H
#define GENERATE_PLAN_H

#include "ProgramLib.h"

// ���з���������
class PlanGenerator {
private:
	// �ڽӱ�߽��
	class arcNode {
	public:
		int verID;					// �յ㶥����
		DangerIndex dangerIndex;	// ��Ȩֵ���˴�Ϊ����ֵ
		arcNode* nextArc;			// ��ñ߾�����ͬ��㶥�����һ����
		arcNode() = delete;
		arcNode(int verID_, DangerIndex dangerIndex_)
			: verID(verID_), dangerIndex(dangerIndex_), nextArc(nullptr) {
		}
	};
	// �ڽӱ�����
	class verNode {
	public:
		int verID;					// ������
		arcNode* firstArc;			// �Ըö���Ϊ���ĵ�һ����
	};
	// ����ڵ㣬���� ID �͵�Դ����Ĺ�����룬���ȶ�����Ҫ������
	class tmpNode {
	public:
		int verID;					// Դ������
		DangerIndex dangerIndex;	// �������
		// ��Ҫʵ����С�ѣ����������У������Ҫ������������ض������ȼ�����СΪ��
		friend bool operator <(tmpNode a, tmpNode b) {
			return a.dangerIndex > b.dangerIndex;
		}
	};
	int nodeNum;					// ����ͼ�ܶ�����
	int** nodeID;					// ������������
	verNode* verList;				// �����б�
public:
	PlanGenerator();
	~PlanGenerator();
	// ����������֮�����һ��ָ��Ȩֵ�ı�
	void addArcNode(int startVer_, int endVer_, DangerIndex dangerIndex_);
	// �������в����������з���
	Plan* generatePlan(
		CityID beginID_, CityID endID_,
		Time beginTime_, Time& lastTime_,
		DangerIndex& finalDangerIndex_, Time limitTime_ = INS_TIME_MAX);
	CityID node2CityID(int nodeID_);// ���ݶ�������������
	Time node2Time(int nodeID_);	// ���ݶ�������ʱ��
};

#endif // !GENERATE_PLAN_H