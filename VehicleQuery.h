#ifndef VEHICLE_QUERY_H
#define VEHICLE_QUERY_H

#include "ui_VehicleQuery.h"
#include "ProgramLib.h"

// ����/�г�/������ѯ����
class VehicleQuery: public QWidget {
	Q_OBJECT
public:
	VehicleQuery(QWidget* parent = Q_NULLPTR);
	~VehicleQuery();
private:
	Ui::VehicleQuery ui;
	QList<QTreeWidgetItem*> vehList;	// ��ͨ������ʾ�б�
private slots:
	void doQuery();				// ��Ӧ"��ѯ"��ť����¼�
};

#endif // !VEHICLE_QUERY_H