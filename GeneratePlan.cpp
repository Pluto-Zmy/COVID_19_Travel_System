#include "Control.h"
#include "DataStructure.h"
#include "GeneratePlan.h"

extern Control System;

// ��ʼ���㷨�������ݽṹ
PlanGenerator::PlanGenerator()
	: nodeID(nullptr), verList(nullptr), nodeNum(System.cityNum* INS_TIME_MAX) {
	// ��ʼ�������
	nodeID = new int* [System.cityNum]{nullptr};
	for (CityID cityID{0}; cityID < System.cityNum; cityID++) {
		nodeID[cityID] = new int[INS_TIME_MAX]();
	}
	for (CityID cityID{0}; cityID < System.cityNum; cityID++) {
		for (Time time{0}; time < INS_TIME_MAX; time++) {
			nodeID[cityID][time] = cityID * INS_TIME_MAX + time;
		}
	}
	// ��ʼ���ڽӱ�
	verList = new verNode[nodeNum]();
	for (int i{0}; i < nodeNum; i++) {
		verList[i].verID = i;
		verList[i].firstArc = nullptr;
	}
	// ��ͬһ��������ʱ�����һ��ȨֵΪ��λ���յı�
	for (CityID cityID{0}; cityID < System.cityNum; cityID++) {
		for (Time time{0}; time < INS_TIME_MAX - 1; time++) {
			addArcNode(nodeID[cityID][time], nodeID[cityID][time + 1],
				System.IDtoCity(cityID)->getCityDangerIndex(1));
		}
	}
	// ����ʱ�̱�������Ӧ�ı�
	for (auto veh : System.schedule) {
		Time startTime = veh->getStartTime(),
			arriveTime = veh->getStartTime() + veh->getRunTime();
		CityID srcID = veh->getSrcCity()->getID(),
			dstID = veh->getDstCity()->getID();
		// ע�⳵�ε�������
		while (arriveTime < INS_TIME_MAX) {
			addArcNode(nodeID[srcID][startTime], nodeID[dstID][arriveTime],
				veh->getVehDangerIndex());
			startTime += 24;
			arriveTime += 24;
		}
	}
}

// �ͷ����ݽṹ��Դ
PlanGenerator::~PlanGenerator() {
	arcNode* tmpNode{nullptr};
	for (int i{0}; i < nodeNum; i++) {
		if (verList[i].firstArc) {
			tmpNode = verList[i].firstArc->nextArc;
			while (tmpNode) {
				delete verList[i].firstArc;
				verList[i].firstArc = tmpNode;
				tmpNode = tmpNode->nextArc;
			}
			delete verList[i].firstArc;
		}
	}
	for (CityID cityID{0}; cityID < System.cityNum; cityID++) {
		delete[] nodeID[cityID];
	}
	delete[] nodeID;
}

// ��ӱߺ���
void PlanGenerator::addArcNode(int startVer_, int endVer_, DangerIndex dangerIndex_) {
	arcNode* newArc = new arcNode(endVer_, dangerIndex_);
	if (nullptr == verList[startVer_].firstArc) {
		verList[startVer_].firstArc = newArc;
	}
	else {
		arcNode* originFirstArc = verList[startVer_].firstArc;
		newArc->nextArc = originFirstArc;
		verList[startVer_].firstArc = newArc;
	}
}

// ���з������ɺ���
Plan* PlanGenerator::generatePlan(
	CityID beginID_, CityID endID_,
	Time beginTime_, Time& lastTime_,
	DangerIndex& finalDangerIndex_, Time limitTime_) {
	tmpNode* dangerIndex = new tmpNode[nodeNum]();
	int* nodePath = new int[nodeNum]();
	int minNode{0};
	bool* nodeVisited = new bool[nodeNum]();
	std::priority_queue<tmpNode> nodeQueue;
	CityID currentCityID{-1};
	DangerIndex minDis{0};
	std::vector<int> bestPath;
	std::vector<int> cityPath;
	Plan* plan{nullptr};
	// ��ʼ��
	for (int i{0}; i < nodeNum; i++) {
		dangerIndex[i].verID = i;
		// ��������� DANGER_MAX
		dangerIndex[i].dangerIndex = DANGER_MAX;
		// ÿ�����㶼�޸��׽ڵ�
		nodePath[i] = -1;
		// ��δ�ҵ����·
		nodeVisited[i] = false;
	}
	// Դ�㵽Դ�����·ȨֵΪ 0
	dangerIndex[nodeID[beginID_][beginTime_]].dangerIndex = 0;
	// ѹ�����ȶ�����
	nodeQueue.push(dangerIndex[nodeID[beginID_][beginTime_]]);
	// ���п�˵������˲���
	while (!nodeQueue.empty()) {
		// ȡ��С������붥��
		tmpNode topNode = nodeQueue.top();
		nodeQueue.pop();
		int topID = topNode.verID;
		// ������ѷ��ʽڵ�Ĳ���Ҫ����
		if (nodeVisited[topID]) {
			continue;
		}
		nodeVisited[topID] = true;
		arcNode* curNode = verList[topID].firstArc;
		// �������������ڵĶ��㣬�����ɳڲ��������¹�����룬ѹ�����
		while (nullptr != curNode) {
			int curID = curNode->verID;
			if (!nodeVisited[curID] &&
				dangerIndex[curID].dangerIndex > dangerIndex[topID].dangerIndex + curNode->dangerIndex) {
				dangerIndex[curID].dangerIndex = dangerIndex[topID].dangerIndex + curNode->dangerIndex;
				nodePath[curID] = topID;
				nodeQueue.push(dangerIndex[curID]);
			}
			curNode = curNode->nextArc;
		}
	}
	// ��ʼ����̾���
	minDis = dangerIndex[nodeID[endID_][beginTime_]].dangerIndex;
	minNode = nodeID[endID_][beginTime_];
	// ����ʱ�����ƣ�������������·��ֵ��ɸѡ������С��
	for (Time time{beginTime_}; time <= min(beginTime_ + limitTime_, INS_TIME_MAX - 1); time++) {
		int i = nodeID[endID_][time];
		if (minDis > dangerIndex[i].dangerIndex) {
			minDis = dangerIndex[i].dangerIndex;
			minNode = i;
		}
	}
	// ��δ�ҵ��������з��������ͷ���Դ�����ؿ�ָ��
	if (DANGER_MAX == dangerIndex[minNode].dangerIndex) {
		delete[] dangerIndex;
		delete[] nodePath;
		delete[] nodeVisited;
		return nullptr;
	}
	// �����ҵ��������з�����׼���Է������з�װ
	plan = new Plan;
	lastTime_ = node2Time(minNode) - beginTime_;
	finalDangerIndex_ = dangerIndex[minNode].dangerIndex;
	// ��һ��ȷ�Ϸ����ɿ���
	if (dangerIndex[minNode].dangerIndex < DANGER_MAX) {
		bestPath.push_back(minNode);
		int curNode = minNode;
		// �����������ڵ㣬����·��
		while (nodePath[curNode] != -1) {
			bestPath.push_back(nodePath[curNode]);
			curNode = nodePath[curNode];
		}
		bestPath.push_back(nodeID[beginID_][beginTime_]);
	}
	// ��·����������
	for (auto bestNode : bestPath) {
		if (node2CityID(bestNode) != currentCityID) {
			cityPath.push_back(bestNode);
			currentCityID = node2CityID(bestNode);
		}
	}
	// ����·���任Ϊ����·��
	reverse(cityPath.begin(), cityPath.end());
	// ����·��Ѱ�Ҷ�Ӧ�ĳ���
	for (auto cityIter = cityPath.begin(); cityIter != (cityPath.end() - 1); cityIter++) {
		City* srcCity = System.IDtoCity(node2CityID(*cityIter)),
			* dstCity = System.IDtoCity(node2CityID(*(cityIter + 1)));
		Time startTime = node2Time(*cityIter) % 24;
		// �����ҵ����Σ��򲻼�������
		for (auto veh : System.schedule) {
			if (veh->getSrcCity() == srcCity &&
				veh->getDstCity() == dstCity &&
				veh->getStartTime() == startTime) {
				plan->push_back(veh);
				break;
			}
		}
	}
	// �ͷ���Դ
	delete[] dangerIndex;
	delete[] nodePath;
	delete[] nodeVisited;
	return plan->empty() ? nullptr : plan;
}

// ���ݶ�������������
CityID PlanGenerator::node2CityID(int nodeID_) {
	return nodeID_ / INS_TIME_MAX;
}

// ���ݶ�������ʱ��
Time PlanGenerator::node2Time(int nodeID_) {
	return nodeID_ % INS_TIME_MAX;
}