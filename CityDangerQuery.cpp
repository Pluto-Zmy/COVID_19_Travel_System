#include "CityDangerQuery.h"
#include "COVID_19_Travel_System.h"
#include "Control.h"
#include "DataStructure.h"

extern Control System;
extern COVID_19_Travel_System* MainWindow;

CityDangerQuery::CityDangerQuery(QWidget* parent)
	: QWidget(parent) {
	ui.setupUi(this);
	// ���õ�ǰ���ڴ�ʱ������������
	setAttribute(Qt::WA_DeleteOnClose);
	// ���������б���ʾ�����Ϣ
	ui.treeWidgetDangerList->setColumnWidth(0, 180);
	for (auto city : System.cityList) {
		QTreeWidgetItem* newInsert = new QTreeWidgetItem;
		newInsert->setText(0, QString::fromLocal8Bit(city->getName().data()));
		newInsert->setText(1, QString::fromLocal8Bit(System.dangerStr[city->getKind()].data()));
		newInsert->setSizeHint(0, QSize(0, 24));
		newInsert->setSizeHint(1, QSize(0, 24));
		cityListUI.append(newInsert);
		ui.treeWidgetDangerList->insertTopLevelItem(cityListUI.size() - 1, newInsert);
	}
}

CityDangerQuery::~CityDangerQuery() {
	for (auto city : cityListUI) {
		delete city;
	}
	cityListUI.clear();
	// �Ӵ��ڹرպ��֪������ʱ�Ӽ���
	MainWindow->callMainUIProceed();
}