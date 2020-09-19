#include "COVID_19_Travel_System.h"
#include "PlanViewer.h"
#include "Map.h"
#include "VehicleQuery.h"
#include "CityDangerQuery.h"
#include "Control.h"
#include "DataStructure.h"

extern Control System;

// 初始化主界面
COVID_19_Travel_System::COVID_19_Travel_System(QWidget* parent)
	: QMainWindow(parent) {
	ui.setupUi(this);
	// 将界面控件与对应的响应方法连接
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
	// 系统时间显示置 0
	ui.lineEditDay->setText(QString::number(0));
	ui.lineEditHour->setText(QString::number(0));
	// 旅客编号置 0
	ui.lineEditTravelID->setText(QString::number(System.instanceNum));
	// 系统暂停
	UIPause();
	// 隐藏进度条
	ui.progressBar->setVisible(false);
	// 初始化城市列表下拉框
	for (auto city : System.cityList) {
		ui.comboBoxBeginID->addItem(QString::fromLocal8Bit(city->getName().data()));
		ui.comboBoxEndID->addItem(QString::fromLocal8Bit(city->getName().data()));
	}
	// 启动 UI 刷新器子线程
	std::thread* UIRefresher = new std::thread(&COVID_19_Travel_System::startUIRefresher, this);
	UIRefresher->detach();
}

// 子窗体关闭时告知主窗体时钟继续
void COVID_19_Travel_System::callMainUIProceed() {
	UIProceed();
}

// 获取 UI 当前选择的旅客编号
int COVID_19_Travel_System::getCurInsIndex() {
	return ui.comboBoxTravelID->currentIndex();
}

// 供其它窗口访问主窗口元素
Ui::COVID_19_Travel_SystemClass* COVID_19_Travel_System::getUI() {
	return &ui;
}

// 若系统不处于暂停状态，以中精度时钟频率发射刷新主界面的信号
void COVID_19_Travel_System::startUIRefresher() {
	while (true) {
		Sleep(CLOCK_FREQ_MEDIUM);
		if (!System.isPaused()) {
			sendRefreshSignal();
		}
	}
}

// 刷新 UI 时间显示，包括"天"和"小时"
void COVID_19_Travel_System::refreshUITime() {
	ui.lineEditHour->setText(QString::number(System.sysTimeNormal % 24));
	ui.lineEditDay->setText(QString::number(System.sysTimeNormal / 24));
}

// 若旅客列表不为空，则刷新当前下拉框选择的旅客的旅行信息
void COVID_19_Travel_System::refreshInfo() {
	TravelInstance* ins{nullptr};
	if (!System.instanceList.empty()) {
		ins = *(System.instanceList.begin() + ui.comboBoxTravelID->currentIndex());
		// 需要刷新的信息包括：1.旅客位置 2.已用时间 3.剩余时间
		ui.textBrowserStatus->setText(QString::fromLocal8Bit(ins->getStatusUI(System.sysTimeNormal).data()));
		// 旅行开始之后才刷新；否则显示 "-"
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

// 刷新主界面，调用刷新时钟显示和刷新当前旅客信息的方法
void COVID_19_Travel_System::refreshUI() {
	refreshUITime();
	refreshInfo();
}

// UI 需要暂停时调用
void COVID_19_Travel_System::UIPause() {
	// 暂停系统时钟
	System.pause();
	// 禁用"暂停"按钮并启用"继续"按钮
	ui.pushButtonPause->setEnabled(false);
	ui.pushButtonProceed->setEnabled(true);
	// 系统暂停时才允许用户输入
	ui.ButtonSubmit->setEnabled(true);
	ui.comboBoxBeginID->setEnabled(true);
	ui.comboBoxEndID->setEnabled(true);
	ui.checkBoxCustomizeBeginTime->setEnabled(true);
	setLimitStatus();
	ui.checkBoxLimit->setEnabled(true);
	setCustomizeBeginTimeStatus();
}

// UI 需要继续时调用
void COVID_19_Travel_System::UIProceed() {
	// 继续系统时钟
	System.proceed();
	// 启用"暂停"按钮并禁用"继续"按钮
	ui.pushButtonPause->setEnabled(true);
	ui.pushButtonProceed->setEnabled(false);
	// 系统运行时禁止用户输入
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

// 响应"自定义起始时间"复选框
void COVID_19_Travel_System::setCustomizeBeginTimeStatus() {
	// 如果复选框被勾选，则启用自定义起始时间输入框；否则禁用它
	ui.spinBoxBeginDay->setEnabled(ui.checkBoxCustomizeBeginTime->isChecked());
	ui.spinBoxBeginHour->setEnabled(ui.checkBoxCustomizeBeginTime->isChecked());
}

// 响应"限制旅行时间"复选框
void COVID_19_Travel_System::setLimitStatus() {
	// 如果复选框被勾选，则启用限时输入框；否则禁用它
	ui.spinBoxLimitDay->setEnabled(ui.checkBoxLimit->isChecked());
	ui.spinBoxLimitHour->setEnabled(ui.checkBoxLimit->isChecked());
}

// 响应"提交旅行信息"点击事件
void COVID_19_Travel_System::onPressSubmit() {
	// 点击之后先暂停时钟
	Time customizeBeginTime = ui.spinBoxBeginDay->value() * 24 + ui.spinBoxBeginHour->value();
	// 如果用户勾选了"自定义开始时间"复选框，但是输入的时间不合法，则驳回请求
	if (ui.checkBoxCustomizeBeginTime->isChecked() && customizeBeginTime < System.sysTimeNormal) {
		QString dlgTitle = QString::fromLocal8Bit("提示");
		QString strInfo = QString::fromLocal8Bit("尊敬的旅客，您设置的自定义旅行起始时间已过。\n请您重新设置。");
		QMessageBox::information(this, dlgTitle, strInfo, QString::fromLocal8Bit("确定"));
	}
	else {
		// 如果用户选择的旅行起点和旅行终点相同，则弹出提示框，提示旅客重新选择
		if (ui.comboBoxBeginID->currentIndex() == ui.comboBoxEndID->currentIndex()) {
			QString dlgTitle = QString::fromLocal8Bit("提示");
			QString strInfo = QString::fromLocal8Bit("尊敬的旅客，旅行起点不能和旅行终点相同。\n请您重新选择。");
			QMessageBox::information(this, dlgTitle, strInfo, QString::fromLocal8Bit("确定"));
		}
		else {
			InputData* uiInput{nullptr};
			Time beginTime{System.sysTimeNormal};
			// 如果用户勾选了"自定义开始时间"复选框，则不采用系统时间，而采用用户输入的时间
			if (ui.checkBoxCustomizeBeginTime->isChecked()) {
				beginTime = customizeBeginTime;
			}
			// 如果用户勾选了"限制旅行时间"复选框，则需要从限时输入框读取限制时间，并传递
			if (ui.checkBoxLimit->isChecked()) {
				Time limitTime = ui.spinBoxLimitDay->value() * 24 + ui.spinBoxLimitHour->value();
				uiInput = new InputData(ui.comboBoxBeginID->currentIndex(),
					ui.comboBoxEndID->currentIndex(), beginTime, LIMIT, limitTime);
			}
			else {
				uiInput = new InputData(ui.comboBoxBeginID->currentIndex(),
					ui.comboBoxEndID->currentIndex(), beginTime, NOLIMIT);
			}
			// 如果旅行创建成功，则处理 UI 操作；否则弹出提示框，要求旅客修改信息
			if (System.addTravelInstance(*uiInput)) {
				// 刷新下一个旅客编号
				ui.lineEditTravelID->setText(QString::number(System.instanceNum));
				// 将新创建的旅客编号添加至下拉框并切换至此
				ui.comboBoxTravelID->addItem(QString::number(System.instanceNum - 1));
				ui.comboBoxTravelID->setCurrentIndex(System.instanceNum - 1);
				// 若旅行终点为高风险地区，则给出安全提示
				if (System.IDtoCity(ui.comboBoxEndID->currentIndex())->getKind() == HIGH) {
					QString dlgTitle = QString::fromLocal8Bit("提示");
					QString strInfo
						= QString::fromLocal8Bit("尊敬的旅客，您此次旅行的终点为高风险地区。请您注意安全，做好个人防护。\n点击\"确定\"查看旅行方案。");
					QMessageBox::information(this, dlgTitle, strInfo, QString::fromLocal8Bit("确定"));
				}
				// 准备弹出旅行结果查询窗口
				PlanViewer* planViewer = new PlanViewer(*(*(System.instanceList.end() - 1)));
				planViewer->setWindowFlags(planViewer->windowFlags() & ~Qt::WindowMinMaxButtonsHint);
				planViewer->setWindowModality(Qt::ApplicationModal);
				planViewer->show();
			}
			else {
				QString dlgTitle = QString::fromLocal8Bit("提示");
				QString strInfo = QString::fromLocal8Bit("尊敬的旅客，当前条件下未查询到合适旅行方案。\n请您调整旅行计划。");
				QMessageBox::information(this, dlgTitle, strInfo, QString::fromLocal8Bit("确定"));
			}
			delete uiInput;
		}
	}
}

// 响应"查看详细信息"点击事件，弹出旅行方案回顾窗口
void COVID_19_Travel_System::onPressViewDetails() {
	UIPause();
	// 旅行列表非空时，显示当前选中的旅客的详细信息；否则弹出提示
	if (!System.instanceList.empty()) {
		PlanViewer* planViewer = new PlanViewer(**(System.instanceList.begin() + ui.comboBoxTravelID->currentIndex()));
		planViewer->setWindowFlags(planViewer->windowFlags() & ~Qt::WindowMinMaxButtonsHint);
		planViewer->setWindowModality(Qt::ApplicationModal);
		planViewer->show();
	}
	else {
		QString dlgTitle = QString::fromLocal8Bit("提示");
		QString strInfo = QString::fromLocal8Bit("尚未创建任何旅行实例。");
		QMessageBox::information(this, dlgTitle, strInfo, QString::fromLocal8Bit("确定"));
		UIProceed();
	}
}

// 响应"航班/列车/汽车查询"点击事件，打开相应窗口
void COVID_19_Travel_System::onPressVehicleQuery() {
	UIPause();
	VehicleQuery* vehQuery = new VehicleQuery;
	vehQuery->setWindowFlags(vehQuery->windowFlags() & ~Qt::WindowMinMaxButtonsHint);
	vehQuery->setWindowModality(Qt::ApplicationModal);
	vehQuery->show();
}

// 响应"城市风险等级查询"点击事件，打开相应窗口
void COVID_19_Travel_System::onPressCityDangerQuery() {
	UIPause();
	CityDangerQuery* dangerQuery = new CityDangerQuery;
	dangerQuery->setWindowFlags(dangerQuery->windowFlags() & ~Qt::WindowMinMaxButtonsHint);
	dangerQuery->setWindowModality(Qt::ApplicationModal);
	dangerQuery->show();
}

// 响应"技术支持"点击事件
void COVID_19_Travel_System::onPressSupport() {
	System.openSupport();
}