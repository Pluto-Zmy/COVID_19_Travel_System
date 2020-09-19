#ifndef VEHICLE_QUERY_H
#define VEHICLE_QUERY_H

#include "ui_VehicleQuery.h"
#include "ProgramLib.h"

// 航班/列车/汽车查询窗口
class VehicleQuery: public QWidget {
	Q_OBJECT
public:
	VehicleQuery(QWidget* parent = Q_NULLPTR);
	~VehicleQuery();
private:
	Ui::VehicleQuery ui;
	QList<QTreeWidgetItem*> vehList;	// 交通工具显示列表
private slots:
	void doQuery();				// 响应"查询"按钮点击事件
};

#endif // !VEHICLE_QUERY_H