#ifndef CITY_DANGER_QUERY_H
#define CITY_DANGER_QUERY_H

#include "ui_CityDangerQuery.h"
#include "ProgramLib.h"

// ���з��յȼ���ѯ����
class CityDangerQuery: public QWidget {
	Q_OBJECT
public:
	CityDangerQuery(QWidget* parent = Q_NULLPTR);
	~CityDangerQuery();
private:
	Ui::CityDangerQuery ui;
	QList<QTreeWidgetItem*> cityListUI;	// ������ʾ�б�
};

#endif // !CITY_DANGER_QUERY_H