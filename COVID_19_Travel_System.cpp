#include "COVID_19_Travel_System.h"
#include "PlanViewer.h"
#include "Map.h"
#include "VehicleQuery.h"
#include "CityDangerQuery.h"
#include "Control.h"
#include "DataStructure.h"

extern Control System;

// ��ʼ��������
COVID_19_Travel_System::COVID_19_Travel_System(QWidget* parent)
	: QMainWindow(parent) {
	ui.setupUi(this);
	// ������ؼ����Ӧ����Ӧ��������
	connect(ui.checkBoxCustomizeBeginTime, &QCheckBox::stateChanged, this, &COVID_19_Travel_System::setCustomizeBeginTimeStatus);
	connect(ui.checkBoxLimit, &QCheckBox::stateChanged, this, &COVID_19_Travel_System::setLimitStatus);
	connect(ui.ButtonSubmit, &QPushButton::clicked, this, &COVID_19_Travel_System::onPressSubmit);
	connect(ui.comboBoxTravelID, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshInfo()));
	connect(ui.ButtonViewDetails, &QPushButton::clicked, this, &COVID_19_Travel_System::onPressViewDetails);
	connect(ui.pushButtonPause, &QPushButton::clicked, this, &COVID_19_Travel_System::UIPause);
	connect(ui.pushButtonProceed, &QPushButton::clicked, this, &COVID_19_Travel_System::UIProceed);
	connect(ui.pushButtonVehicleQuery, &QPushButton::clicked, this, &COVID_19_Travel_System::onPressVehicleQuery);
	connect(ui.pushButtonCityDangerQuery, &QPushButton::clicked, this, &COVID_19_Travel_System::onPressCityDangerQuery);
	connect(ui.pushButtonSupport, &QPushButton::clicked, this, &COVID_19_Travel_System::onPressSupport);
	connect(this, &COVID_19_Travel_System::sendRefreshSignal, this, &COVID_19_Travel_System::refreshUI);
	// ϵͳʱ����ʾ�� 0
	ui.lineEditDay->setText(QString::number(0));
	ui.lineEditHour->setText(QString::number(0));
	// �ÿͱ���� 0
	ui.lineEditTravelID->setText(QString::number(System.instanceNum));
	// ϵͳ��ͣ
	UIPause();
	// ���ؽ�����
	ui.progressBar->setVisible(false);
	// ��ʼ�������б�������
	for (auto city : System.cityList) {
		ui.comboBoxBeginID->addItem(QString::fromLocal8Bit(city->getName().data()));
		ui.comboBoxEndID->addItem(QString::fromLocal8Bit(city->getName().data()));
	}
	// ���� UI ˢ�������߳�
	std::thread* UIRefresher = new std::thread(&COVID_19_Travel_System::startUIRefresher, this);
	UIRefresher->detach();
}

// �Ӵ���ر�ʱ��֪������ʱ�Ӽ���
void COVID_19_Travel_System::callMainUIProceed() {
	UIProceed();
}

// ��ȡ UI ��ǰѡ����ÿͱ��
int COVID_19_Travel_System::getCurInsIndex() {
	return ui.comboBoxTravelID->currentIndex();
}

// ���������ڷ���������Ԫ��
Ui::COVID_19_Travel_SystemClass* COVID_19_Travel_System::getUI() {
	return &ui;
}

// ��ϵͳ��������ͣ״̬�����о���ʱ��Ƶ�ʷ���ˢ����������ź�
void COVID_19_Travel_System::startUIRefresher() {
	while (true) {
		Sleep(CLOCK_FREQ_MEDIUM);
		if (!System.isPaused()) {
			sendRefreshSignal();
		}
	}
}

// ˢ�� UI ʱ����ʾ������"��"��"Сʱ"
void COVID_19_Travel_System::refreshUITime() {
	ui.lineEditHour->setText(QString::number(System.sysTimeNormal % 24));
	ui.lineEditDay->setText(QString::number(System.sysTimeNormal / 24));
}

// ���ÿ��б�Ϊ�գ���ˢ�µ�ǰ������ѡ����ÿ͵�������Ϣ
void COVID_19_Travel_System::refreshInfo() {
	TravelInstance* ins{nullptr};
	if (!System.instanceList.empty()) {
		ins = *(System.instanceList.begin() + ui.comboBoxTravelID->currentIndex());
		// ��Ҫˢ�µ���Ϣ������1.�ÿ�λ�� 2.����ʱ�� 3.ʣ��ʱ��
		ui.textBrowserStatus->setText(QString::fromLocal8Bit(ins->getStatusUI(System.sysTimeNormal).data()));
		// ���п�ʼ֮���ˢ�£�������ʾ "-"
		if (System.sysTimeNormal >= ins->getBeginTime()) {
			ui.lineEditUsedDay->setText(QString::number(min((System.sysTimeNormal - ins->getBeginTime()), ins->getLastTime()) / 24));
			ui.lineEditUsedHour->setText(QString::number(min((System.sysTimeNormal - ins->getBeginTime()), ins->getLastTime()) % 24));
			ui.lineEditRestDay->setText(QString::number(max((ins->getLastTime() + ins->getBeginTime() - System.sysTimeNormal), 0) / 24));
			ui.lineEditRestHour->setText(QString::number(max((ins->getLastTime() + ins->getBeginTime() - System.sysTimeNormal), 0) % 24));
		}
		else {
			ui.lineEditUsedDay->setText("-");
			ui.lineEditUsedHour->setText("-");
			ui.lineEditRestDay->setText("-");
			ui.lineEditRestHour->setText("-");
		}
	}
}

// ˢ�������棬����ˢ��ʱ����ʾ��ˢ�µ�ǰ�ÿ���Ϣ�ķ���
void COVID_19_Travel_System::refreshUI() {
	refreshUITime();
	refreshInfo();
}

// UI ��Ҫ��ͣʱ����
void COVID_19_Travel_System::UIPause() {
	// ��ͣϵͳʱ��
	System.pause();
	// ����"��ͣ"��ť������"����"��ť
	ui.pushButtonPause->setEnabled(false);
	ui.pushButtonProceed->setEnabled(true);
	// ϵͳ��ͣʱ�������û�����
	ui.ButtonSubmit->setEnabled(true);
	ui.comboBoxBeginID->setEnabled(true);
	ui.comboBoxEndID->setEnabled(true);
	ui.checkBoxCustomizeBeginTime->setEnabled(true);
	setLimitStatus();
	ui.checkBoxLimit->setEnabled(true);
	setCustomizeBeginTimeStatus();
}

// UI ��Ҫ����ʱ����
void COVID_19_Travel_System::UIProceed() {
	// ����ϵͳʱ��
	System.proceed();
	// ����"��ͣ"��ť������"����"��ť
	ui.pushButtonPause->setEnabled(true);
	ui.pushButtonProceed->setEnabled(false);
	// ϵͳ����ʱ��ֹ�û�����
	ui.ButtonSubmit->setEnabled(false);
	ui.comboBoxBeginID->setEnabled(false);
	ui.comboBoxEndID->setEnabled(false);
	ui.checkBoxCustomizeBeginTime->setEnabled(false);
	ui.spinBoxBeginDay->setEnabled(false);
	ui.spinBoxBeginHour->setEnabled(false);
	ui.checkBoxLimit->setEnabled(false);
	ui.spinBoxLimitDay->setEnabled(false);
	ui.spinBoxLimitHour->setEnabled(false);
}

// ��Ӧ"�Զ�����ʼʱ��"��ѡ��
void COVID_19_Travel_System::setCustomizeBeginTimeStatus() {
	// �����ѡ�򱻹�ѡ���������Զ�����ʼʱ������򣻷��������
	ui.spinBoxBeginDay->setEnabled(ui.checkBoxCustomizeBeginTime->isChecked());
	ui.spinBoxBeginHour->setEnabled(ui.checkBoxCustomizeBeginTime->isChecked());
}

// ��Ӧ"��������ʱ��"��ѡ��
void COVID_19_Travel_System::setLimitStatus() {
	// �����ѡ�򱻹�ѡ����������ʱ����򣻷��������
	ui.spinBoxLimitDay->setEnabled(ui.checkBoxLimit->isChecked());
	ui.spinBoxLimitHour->setEnabled(ui.checkBoxLimit->isChecked());
}

// ��Ӧ"�ύ������Ϣ"����¼�
void COVID_19_Travel_System::onPressSubmit() {
	// ���֮������ͣʱ��
	Time customizeBeginTime = ui.spinBoxBeginDay->value() * 24 + ui.spinBoxBeginHour->value();
	// ����û���ѡ��"�Զ��忪ʼʱ��"��ѡ�򣬵��������ʱ�䲻�Ϸ����򲵻�����
	if (ui.checkBoxCustomizeBeginTime->isChecked() && customizeBeginTime < System.sysTimeNormal) {
		QString dlgTitle = QString::fromLocal8Bit("��ʾ");
		QString strInfo = QString::fromLocal8Bit("�𾴵��ÿͣ������õ��Զ���������ʼʱ���ѹ���\n�����������á�");
		QMessageBox::information(this, dlgTitle, strInfo, QString::fromLocal8Bit("ȷ��"));
	}
	else {
		// ����û�ѡ����������������յ���ͬ���򵯳���ʾ����ʾ�ÿ�����ѡ��
		if (ui.comboBoxBeginID->currentIndex() == ui.comboBoxEndID->currentIndex()) {
			QString dlgTitle = QString::fromLocal8Bit("��ʾ");
			QString strInfo = QString::fromLocal8Bit("�𾴵��ÿͣ�������㲻�ܺ������յ���ͬ��\n��������ѡ��");
			QMessageBox::information(this, dlgTitle, strInfo, QString::fromLocal8Bit("ȷ��"));
		}
		else {
			InputData* uiInput{nullptr};
			Time beginTime{System.sysTimeNormal};
			// ����û���ѡ��"�Զ��忪ʼʱ��"��ѡ���򲻲���ϵͳʱ�䣬�������û������ʱ��
			if (ui.checkBoxCustomizeBeginTime->isChecked()) {
				beginTime = customizeBeginTime;
			}
			// ����û���ѡ��"��������ʱ��"��ѡ������Ҫ����ʱ������ȡ����ʱ�䣬������
			if (ui.checkBoxLimit->isChecked()) {
				Time limitTime = ui.spinBoxLimitDay->value() * 24 + ui.spinBoxLimitHour->value();
				uiInput = new InputData(ui.comboBoxBeginID->currentIndex(),
					ui.comboBoxEndID->currentIndex(), beginTime, LIMIT, limitTime);
			}
			else {
				uiInput = new InputData(ui.comboBoxBeginID->currentIndex(),
					ui.comboBoxEndID->currentIndex(), beginTime, NOLIMIT);
			}
			// ������д����ɹ������� UI ���������򵯳���ʾ��Ҫ���ÿ��޸���Ϣ
			if (System.addTravelInstance(*uiInput)) {
				// ˢ����һ���ÿͱ��
				ui.lineEditTravelID->setText(QString::number(System.instanceNum));
				// ���´������ÿͱ��������������л�����
				ui.comboBoxTravelID->addItem(QString::number(System.instanceNum - 1));
				ui.comboBoxTravelID->setCurrentIndex(System.instanceNum - 1);
				// �������յ�Ϊ�߷��յ������������ȫ��ʾ
				if (System.IDtoCity(ui.comboBoxEndID->currentIndex())->getKind() == HIGH) {
					QString dlgTitle = QString::fromLocal8Bit("��ʾ");
					QString strInfo
						= QString::fromLocal8Bit("�𾴵��ÿͣ����˴����е��յ�Ϊ�߷��յ���������ע�ⰲȫ�����ø��˷�����\n���\"ȷ��\"�鿴���з�����");
					QMessageBox::information(this, dlgTitle, strInfo, QString::fromLocal8Bit("ȷ��"));
				}
				// ׼���������н����ѯ����
				PlanViewer* planViewer = new PlanViewer(*(*(System.instanceList.end() - 1)));
				planViewer->setWindowFlags(planViewer->windowFlags() & ~Qt::WindowMinMaxButtonsHint);
				planViewer->setWindowModality(Qt::ApplicationModal);
				planViewer->show();
			}
			else {
				QString dlgTitle = QString::fromLocal8Bit("��ʾ");
				QString strInfo = QString::fromLocal8Bit("�𾴵��ÿͣ���ǰ������δ��ѯ���������з�����\n�����������мƻ���");
				QMessageBox::information(this, dlgTitle, strInfo, QString::fromLocal8Bit("ȷ��"));
			}
			delete uiInput;
		}
	}
}

// ��Ӧ"�鿴��ϸ��Ϣ"����¼����������з����ع˴���
void COVID_19_Travel_System::onPressViewDetails() {
	UIPause();
	// �����б�ǿ�ʱ����ʾ��ǰѡ�е��ÿ͵���ϸ��Ϣ�����򵯳���ʾ
	if (!System.instanceList.empty()) {
		PlanViewer* planViewer = new PlanViewer(**(System.instanceList.begin() + ui.comboBoxTravelID->currentIndex()));
		planViewer->setWindowFlags(planViewer->windowFlags() & ~Qt::WindowMinMaxButtonsHint);
		planViewer->setWindowModality(Qt::ApplicationModal);
		planViewer->show();
	}
	else {
		QString dlgTitle = QString::fromLocal8Bit("��ʾ");
		QString strInfo = QString::fromLocal8Bit("��δ�����κ�����ʵ����");
		QMessageBox::information(this, dlgTitle, strInfo, QString::fromLocal8Bit("ȷ��"));
		UIProceed();
	}
}

// ��Ӧ"����/�г�/������ѯ"����¼�������Ӧ����
void COVID_19_Travel_System::onPressVehicleQuery() {
	UIPause();
	VehicleQuery* vehQuery = new VehicleQuery;
	vehQuery->setWindowFlags(vehQuery->windowFlags() & ~Qt::WindowMinMaxButtonsHint);
	vehQuery->setWindowModality(Qt::ApplicationModal);
	vehQuery->show();
}

// ��Ӧ"���з��յȼ���ѯ"����¼�������Ӧ����
void COVID_19_Travel_System::onPressCityDangerQuery() {
	UIPause();
	CityDangerQuery* dangerQuery = new CityDangerQuery;
	dangerQuery->setWindowFlags(dangerQuery->windowFlags() & ~Qt::WindowMinMaxButtonsHint);
	dangerQuery->setWindowModality(Qt::ApplicationModal);
	dangerQuery->show();
}

// ��Ӧ"����֧��"����¼�
void COVID_19_Travel_System::onPressSupport() {
	System.openSupport();
}