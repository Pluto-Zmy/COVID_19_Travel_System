#ifndef CITY_DANGER_QUERY_H
#define CITY_DANGER_QUERY_H

#include "ui_CityDangerQuery.h"
#include "ProgramLib.h"

// 城市风险等级查询窗口
class CityDangerQuery: public QWidget {
	Q_OBJECT
public:
	CityDangerQuery(QWidget* parent = Q_NULLPTR);
	~CityDangerQuery();
private:
	Ui::CityDangerQuery ui;
	QList<QTreeWidgetItem*> cityListUI;	// 城市显示列表
};

#endif // !CITY_DANGER_QUERY_H