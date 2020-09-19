#ifndef COVID_19_TRAVEL_SYSTEM_H
#define COVID_19_TRAVEL_SYSTEM_H

#include "ui_COVID_19_Travel_System.h"
#include "ProgramLib.h"

// 程序主窗口
class COVID_19_Travel_System: public QMainWindow {
	Q_OBJECT
public:
	COVID_19_Travel_System(QWidget* parent = Q_NULLPTR);
	void callMainUIProceed();					// 告知系统时钟继续
	int getCurInsIndex();						// 获取 UI 当前选择的旅旅客编号
	Ui::COVID_19_Travel_SystemClass* getUI();	// 供其它窗口访问主窗口元素
private:
	Ui::COVID_19_Travel_SystemClass ui;
	void startUIRefresher();					// 启动 UI 刷新器子线程
	void refreshUITime();						// 刷新 UI 显示时间
private slots:
	void refreshInfo();							// 刷新 UI 当前选择的旅客信息
	void refreshUI();							// 刷新 UI 信息
	void UIPause();								// 告知控制层暂停时钟
	void UIProceed();							// 告知控制层继续时钟
	void setCustomizeBeginTimeStatus();			// 根据自定义起始时间与否复选框设置起始时间输入框可用状态
	void setLimitStatus();						// 根据限时与否复选框设置限时输入框可用状态
	void onPressSubmit();						// 响应"提交旅行信息"点击事件
	void onPressViewDetails();					// 响应"查看详细信息"点击事件
	void onPressVehicleQuery();					// 响应"航班/列车/汽车查询"点击事件
	void onPressCityDangerQuery();				// 响应"城市风险等级查询"点击事件
	void onPressSupport();						// 响应"技术支持"点击事件
signals:
	void sendRefreshSignal();					// 发送界面刷新信号
};

#endif // !COVID_19_TRAVEL_SYSTEM_H