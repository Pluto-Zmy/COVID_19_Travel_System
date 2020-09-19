#include "Control.h"
#include "DataStructure.h"
#include "GeneratePlan.h"

extern Control System;

// 初始化算法所需数据结构
PlanGenerator::PlanGenerator()
	: nodeID(nullptr), verList(nullptr), nodeNum(System.cityNum* INS_TIME_MAX) {
	// 初始化结点编号
	nodeID = new int* [System.cityNum]{nullptr};
	for (CityID cityID{0}; cityID < System.cityNum; cityID++) {
		nodeID[cityID] = new int[INS_TIME_MAX]();
	}
	for (CityID cityID{0}; cityID < System.cityNum; cityID++) {
		for (Time time{0}; time < INS_TIME_MAX; time++) {
			nodeID[cityID][time] = cityID * INS_TIME_MAX + time;
		}
	}
	// 初始化邻接表
	verList = new verNode[nodeNum]();
	for (int i{0}; i < nodeNum; i++) {
		verList[i].verID = i;
		verList[i].firstArc = nullptr;
	}
	// 在同一城市相邻时刻添加一条权值为单位风险的边
	for (CityID cityID{0}; cityID < System.cityNum; cityID++) {
		for (Time time{0}; time < INS_TIME_MAX - 1; time++) {
			addArcNode(nodeID[cityID][time], nodeID[cityID][time + 1],
				System.IDtoCity(cityID)->getCityDangerIndex(1));
		}
	}
	// 遍历时刻表，添加相对应的边
	for (auto veh : System.schedule) {
		Time startTime = veh->getStartTime(),
			arriveTime = veh->getStartTime() + veh->getRunTime();
		CityID srcID = veh->getSrcCity()->getID(),
			dstID = veh->getDstCity()->getID();
		// 注意车次的周期性
		while (arriveTime < INS_TIME_MAX) {
			addArcNode(nodeID[srcID][startTime], nodeID[dstID][arriveTime],
				veh->getVehDangerIndex());
			startTime += 24;
			arriveTime += 24;
		}
	}
}

// 释放数据结构资源
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

// 添加边函数
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

// 旅行方案生成函数
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
	// 初始化
	for (int i{0}; i < nodeNum; i++) {
		dangerIndex[i].verID = i;
		// 估算距离置 DANGER_MAX
		dangerIndex[i].dangerIndex = DANGER_MAX;
		// 每个顶点都无父亲节点
		nodePath[i] = -1;
		// 都未找到最短路
		nodeVisited[i] = false;
	}
	// 源点到源点最短路权值为 0
	dangerIndex[nodeID[beginID_][beginTime_]].dangerIndex = 0;
	// 压入优先队列中
	nodeQueue.push(dangerIndex[nodeID[beginID_][beginTime_]]);
	// 队列空说明完成了操作
	while (!nodeQueue.empty()) {
		// 取最小估算距离顶点
		tmpNode topNode = nodeQueue.top();
		nodeQueue.pop();
		int topID = topNode.verID;
		// 避免对已访问节点的不必要操作
		if (nodeVisited[topID]) {
			continue;
		}
		nodeVisited[topID] = true;
		arcNode* curNode = verList[topID].firstArc;
		// 找所有与他相邻的顶点，进行松弛操作，更新估算距离，压入队列
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
	// 初始化最短距离
	minDis = dangerIndex[nodeID[endID_][beginTime_]].dangerIndex;
	minNode = nodeID[endID_][beginTime_];
	// 根据时间限制，在已求出的最短路径值中筛选风险最小者
	for (Time time{beginTime_}; time <= min(beginTime_ + limitTime_, INS_TIME_MAX - 1); time++) {
		int i = nodeID[endID_][time];
		if (minDis > dangerIndex[i].dangerIndex) {
			minDis = dangerIndex[i].dangerIndex;
			minNode = i;
		}
	}
	// 若未找到合适旅行方案，则释放资源，返回空指针
	if (DANGER_MAX == dangerIndex[minNode].dangerIndex) {
		delete[] dangerIndex;
		delete[] nodePath;
		delete[] nodeVisited;
		return nullptr;
	}
	// 若已找到合适旅行方案，准备对方案进行封装
	plan = new Plan;
	lastTime_ = node2Time(minNode) - beginTime_;
	finalDangerIndex_ = dangerIndex[minNode].dangerIndex;
	// 进一步确认方案可靠性
	if (dangerIndex[minNode].dangerIndex < DANGER_MAX) {
		bestPath.push_back(minNode);
		int curNode = minNode;
		// 迭代搜索父节点，生成路径
		while (nodePath[curNode] != -1) {
			bestPath.push_back(nodePath[curNode]);
			curNode = nodePath[curNode];
		}
		bestPath.push_back(nodeID[beginID_][beginTime_]);
	}
	// 将路径存入容器
	for (auto bestNode : bestPath) {
		if (node2CityID(bestNode) != currentCityID) {
			cityPath.push_back(bestNode);
			currentCityID = node2CityID(bestNode);
		}
	}
	// 逆序路径变换为正序路径
	reverse(cityPath.begin(), cityPath.end());
	// 根据路径寻找对应的车次
	for (auto cityIter = cityPath.begin(); cityIter != (cityPath.end() - 1); cityIter++) {
		City* srcCity = System.IDtoCity(node2CityID(*cityIter)),
			* dstCity = System.IDtoCity(node2CityID(*(cityIter + 1)));
		Time startTime = node2Time(*cityIter) % 24;
		// 若已找到车次，则不继续遍历
		for (auto veh : System.schedule) {
			if (veh->getSrcCity() == srcCity &&
				veh->getDstCity() == dstCity &&
				veh->getStartTime() == startTime) {
				plan->push_back(veh);
				break;
			}
		}
	}
	// 释放资源
	delete[] dangerIndex;
	delete[] nodePath;
	delete[] nodeVisited;
	return plan->empty() ? nullptr : plan;
}

// 根据顶点编号求城市序号
CityID PlanGenerator::node2CityID(int nodeID_) {
	return nodeID_ / INS_TIME_MAX;
}

// 根据顶点编号求时间
Time PlanGenerator::node2Time(int nodeID_) {
	return nodeID_ % INS_TIME_MAX;
}