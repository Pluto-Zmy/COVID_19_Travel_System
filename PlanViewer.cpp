#include "PlanViewer.h"
#include "COVID_19_Travel_System.h"
#include "Control.h"
#include "DataStructure.h"

extern Control System;
extern COVID_19_Travel_System* MainWindow;

// ��ʼ�����н����ѯ����
PlanViewer::PlanViewer(TravelInstance& ins_, QWidget* parent)
	: QWidget(parent) {
	ui.setupUi(this);
	// ���õ�ǰ���ڴ�ʱ������������
	setAttribute(Qt::WA_DeleteOnClose);
	// ��ʼ��������ʾ�ؼ�
	ui.lineEditStartDay->setText(QString::number(ins_.getBeginTime() / 24));
	ui.lineEditStartHour->setText(QString::number(ins_.getBeginTime() % 24));
	ui.lineEditStartCity->setText(QString::fromLocal8Bit(ins_.getBeginCity()->getName().data()));
	ui.lineEditEndCity->setText(QString::fromLocal8Bit(ins_.getEndCity()->getName().data()));
	ui.lineEditDanger->setText(QString::number((double) ins_.getDangerIndex() / 10.0));
	ui.lineEditUsedDay->setText(QString::number(ins_.getLastTime() / 24));
	ui.lineEditUsedHour->setText(QString::number(ins_.getLastTime() % 24));
	// �������з����������У���������г�������Ϣ
	for (auto vehIter = ins_.getPlan()->begin(); vehIter != ins_.getPlan()->end(); vehIter++) {
		// ����ÿ�೵�εĵ���ʱ��
		Time originTime;
		// �����ʼ���أ��ض���Ϊ���п�ʼʱ�䣻����Ϊʵ�ʵ���ʱ��
		if ((*vehIter)->getSrcCity() == ins_.getBeginCity()) {
			originTime = ins_.getBeginTime() % 24;
		}
		else {
			originTime = (*(vehIter - 1))->getArriveTime();
		}
		// �����������೵�εĵ����뷢�����������ĳ���еĵȴ�ʱ��
		std::string cityPlanStr = ("�� �ڡ�" + (*vehIter)->getSrcCity()->getName() + "�С��ȴ� " +
			std::to_string(System.getWaitTime(originTime, (*vehIter)->getStartTime())) + " Сʱ");
		// ���ڳ˳�״̬����ֱ����ʾ����������Ϣ
		std::string vehPlanStr = ("�� " + (*vehIter)->toUIString());
		// ������������Ϣ������ʱ��˳����ʾ���б���
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

// �ͷŴ�����Դ����Ҫ�Ƕ�̬������б��ڴ�
PlanViewer::~PlanViewer() {
	int counter = ui.listWidgetPlan->count();
	for (int index{0}; index < counter; index++) {
		QListWidgetItem* item = ui.listWidgetPlan->takeItem(0);
		delete item;
	}
	// �Ӵ��ڹرպ��֪������ʱ�Ӽ���
	MainWindow->callMainUIProceed();
}