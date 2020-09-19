#include "VehicleQuery.h"
#include "COVID_19_Travel_System.h"
#include "Control.h"
#include "DataStructure.h"

extern Control System;
extern COVID_19_Travel_System* MainWindow;

VehicleQuery::VehicleQuery(QWidget* parent)
	: QWidget(parent) {
	ui.setupUi(this);
	// ���õ�ǰ���ڴ�ʱ������������
	setAttribute(Qt::WA_DeleteOnClose);
	connect(ui.comboBoxSrcID, SIGNAL(currentIndexChanged(int)), this, SLOT(doQuery()));
	connect(ui.comboBoxDstID, SIGNAL(currentIndexChanged(int)), this, SLOT(doQuery()));
	// ��ʼ�������б�������
	for (auto city : System.cityList) {
		ui.comboBoxSrcID->addItem(QString::fromLocal8Bit(city->getName().data()));
		ui.comboBoxDstID->addItem(QString::fromLocal8Bit(city->getName().data()));
	}
	// �����б���
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
	// �Ӵ��ڹرպ��֪������ʱ�Ӽ���
	MainWindow->callMainUIProceed();
}

void VehicleQuery::doQuery() {
	// ����б�ǿգ�˵����ѯ������Ҫ�ͷž���Դ
	if (!vehList.isEmpty()) {
		for (auto veh : vehList) {
			delete veh;
		}
		vehList.clear();
	}
	// ���ݳ����б�������ɸѡ����
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
			newInsert->setText(6, QString::number(veh->getRunTime()) + QString::fromLocal8Bit(" Сʱ"));
			vehList.append(newInsert);
			ui.treeWidgetSchedule->insertTopLevelItem(vehList.size() - 1, newInsert);
		}
	}
}