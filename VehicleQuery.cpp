#include "VehicleQuery.h"
#include "COVID_19_Travel_System.h"
#include "Control.h"
#include "DataStructure.h"

extern Control System;
extern COVID_19_Travel_System* MainWindow;

VehicleQuery::VehicleQuery(QWidget* parent)
	: QWidget(parent) {
	ui.setupUi(this);
	// 设置当前窗口打开时，主窗口阻塞
	setAttribute(Qt::WA_DeleteOnClose);
	connect(ui.comboBoxSrcID, SIGNAL(currentIndexChanged(int)), this, SLOT(doQuery()));
	connect(ui.comboBoxDstID, SIGNAL(currentIndexChanged(int)), this, SLOT(doQuery()));
	// 初始化城市列表下拉框
	for (auto city : System.cityList) {
		ui.comboBoxSrcID->addItem(QString::fromLocal8Bit(city->getName().data()));
		ui.comboBoxDstID->addItem(QString::fromLocal8Bit(city->getName().data()));
	}
	// 设置列表宽度
	for (int i{0}; i < 7; i++) {
		ui.treeWidgetSchedule->setColumnWidth(i, 70);
	}
}

VehicleQuery::~VehicleQuery() {
	if (!vehList.isEmpty()) {
		for (auto veh : vehList) {
			delete veh;
		}
		vehList.clear();
	}
	// 子窗口关闭后告知主程序时钟继续
	MainWindow->callMainUIProceed();
}

void VehicleQuery::doQuery() {
	// 如果列表非空，说明查询过，需要释放旧资源
	if (!vehList.isEmpty()) {
		for (auto veh : vehList) {
			delete veh;
		}
		vehList.clear();
	}
	// 根据城市列表下拉框筛选车次
	for (auto veh : System.schedule) {
		if (veh->getSrcCity()->getID() == ui.comboBoxSrcID->currentIndex() &&
			veh->getDstCity()->getID() == ui.comboBoxDstID->currentIndex()) {
			QTreeWidgetItem* newInsert = new QTreeWidgetItem;
			newInsert->setText(0, QString::fromLocal8Bit(System.vehKindStr[veh->getKind()].data()));
			newInsert->setText(1, QString::fromLocal8Bit(veh->getName().data()));
			newInsert->setText(2, QString::fromLocal8Bit(veh->getSrcCity()->getName().data()));
			newInsert->setText(3, QString::fromLocal8Bit(veh->getDstCity()->getName().data()));
			newInsert->setText(4, QString::number(veh->getStartTime()) + ":00");
			newInsert->setText(5, QString::number(veh->getArriveTime()) + ":00");
			newInsert->setText(6, QString::number(veh->getRunTime()) + QString::fromLocal8Bit(" 小时"));
			vehList.append(newInsert);
			ui.treeWidgetSchedule->insertTopLevelItem(vehList.size() - 1, newInsert);
		}
	}
}