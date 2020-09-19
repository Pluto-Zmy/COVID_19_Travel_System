#ifndef GENERATE_PLAN_H
#define GENERATE_PLAN_H

#include "ProgramLib.h"

// 旅行方案生成类
class PlanGenerator {
private:
	// 邻接表边结点
	class arcNode {
	public:
		int verID;					// 终点顶点编号
		DangerIndex dangerIndex;	// 边权值，此处为风险值
		arcNode* nextArc;			// 与该边具有相同起点顶点的下一条边
		arcNode() = delete;
		arcNode(int verID_, DangerIndex dangerIndex_)
			: verID(verID_), dangerIndex(dangerIndex_), nextArc(nullptr) {
		}
	};
	// 邻接表顶点结点
	class verNode {
	public:
		int verID;					// 顶点编号
		arcNode* firstArc;			// 以该顶点为起点的第一条边
	};
	// 顶点节点，保存 ID 和到源顶点的估算距离，优先队列需要的类型
	class tmpNode {
	public:
		int verID;					// 源顶点编号
		DangerIndex dangerIndex;	// 估算距离
		// 因要实现最小堆，按升序排列，因而需要重载运算符，重定义优先级，以小为先
		friend bool operator <(tmpNode a, tmpNode b) {
			return a.dangerIndex > b.dangerIndex;
		}
	};
	int nodeNum;					// 风险图总顶点数
	int** nodeID;					// 顶点索引数组
	verNode* verList;				// 顶点列表
public:
	PlanGenerator();
	~PlanGenerator();
	// 在两个顶点之间添加一条指定权值的边
	void addArcNode(int startVer_, int endVer_, DangerIndex dangerIndex_);
	// 根据旅行策略生成旅行方案
	Plan* generatePlan(
		CityID beginID_, CityID endID_,
		Time beginTime_, Time& lastTime_,
		DangerIndex& finalDangerIndex_, Time limitTime_ = INS_TIME_MAX);
	CityID node2CityID(int nodeID_);// 根据顶点编号求城市序号
	Time node2Time(int nodeID_);	// 根据顶点编号求时间
};

#endif // !GENERATE_PLAN_H