#ifndef PLAN_VIEWER_H
#define PLAN_VIEWER_H

#include "ui_PlanViewer.h"
#include "ProgramLib.h"

// ���з�����ѯ�����ʾ����
class PlanViewer: public QWidget {
	Q_OBJECT
public:
	PlanViewer(TravelInstance& ins_, QWidget* parent = Q_NULLPTR);
	~PlanViewer();
private:
	Ui::PlanViewer ui;
};

#endif // !PLAN_VIEWER_H