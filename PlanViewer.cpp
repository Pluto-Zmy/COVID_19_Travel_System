#include "PlanViewer.h"
#include "COVID_19_Travel_System.h"
#include "Control.h"
#include "DataStructure.h"

extern Control System;
extern COVID_19_Travel_System* MainWindow;

// 初始化旅行结果查询窗口
PlanViewer::PlanViewer(TravelInstance& ins_, QWidget* parent)
	: QWidget(parent) {
	ui.setupUi(this);
	// 设置当前窗口打开时，主窗口阻塞
	setAttribute(Qt::WA_DeleteOnClose);
	// 初始化界面显示控件
	ui.lineEditStartDay->setText(QString::number(ins_.getBeginTime() / 24));
	ui.lineEditStartHour->setText(QString::number(ins_.getBeginTime() % 24));
	ui.lineEditStartCity->setText(QString::fromLocal8Bit(ins_.getBeginCity()->getName().data()));
	ui.lineEditEndCity->setText(QString::fromLocal8Bit(ins_.getEndCity()->getName().data()));
	ui.lineEditDanger->setText(QString::number((double) ins_.getDangerIndex() / 10.0));
	ui.lineEditUsedDay->setText(QString::number(ins_.getLastTime() / 24));
	ui.lineEditUsedHour->setText(QString::number(ins_.getLastTime() % 24));
	// 遍历旅行方案车次序列，生成相关行程描述信息
	for (auto vehIter = ins_.getPlan()->begin(); vehIter != ins_.getPlan()->end(); vehIter++) {
		// 定义每班车次的到达时间
		Time originTime;
		// 如果在始发地，特定义为旅行开始时间；否则为实际到达时间
		if ((*vehIter)->getSrcCity() == ins_.getBeginCity()) {
			originTime = ins_.getBeginTime() % 24;
		}
		else {
			originTime = (*(vehIter - 1))->getArriveTime();
		}
		// 计算相邻两班车次的到达与发车间隔，即在某城市的等待时间
		std::string cityPlanStr = ("⊙ 在【" + (*vehIter)->getSrcCity()->getName() + "市】等待 " +
			std::to_string(System.getWaitTime(originTime, (*vehIter)->getStartTime())) + " 小时");
		// 处于乘车状态，则直接显示车次描述信息
		std::string vehPlanStr = ("＞ " + (*vehIter)->toUIString());
		// 将两类描述信息按发生时间顺序显示在列表中
		QString uiCityPlanStr = QString::fromLocal8Bit(cityPlanStr.data());
		QString uiVehPlanStr = QString::fromLocal8Bit(vehPlanStr.data());
		QListWidgetItem* newCityInsert = new QListWidgetItem(uiCityPlanStr);
		QListWidgetItem* newVehInsert = new QListWidgetItem(uiVehPlanStr);
		newCityInsert->setSizeHint(QSize(0, 24));
		newVehInsert->setSizeHint(QSize(0, 24));
		ui.listWidgetPlan->insertItem(ui.listWidgetPlan->count(), newCityInsert);
		ui.listWidgetPlan->insertItem(ui.listWidgetPlan->count(), newVehInsert);
	}
}

// 释放窗口资源，主要是动态申请的列表内存
PlanViewer::~PlanViewer() {
	int counter = ui.listWidgetPlan->count();
	for (int index{0}; index < counter; index++) {
		QListWidgetItem* item = ui.listWidgetPlan->takeItem(0);
		delete item;
	}
	// 子窗口关闭后告知主程序时钟继续
	MainWindow->callMainUIProceed();
}