#ifndef MAP_H
#define MAP_H

#include "ProgramLib.h"

// ʵʱ��ͼģ��
class Map: public QWidget {
	Q_OBJECT
public:
	Map(QWidget* parent = Q_NULLPTR);
	~Map() = default;
private:
	QTimer* rePaintSignal;					// ��ͼˢ�¶�ʱ��
	Status curInsStatus;					// ��ǰ����ʵ����״̬
	void paintEvent(QPaintEvent* event);	// ��ͼ�¼�
	QPixmap getIconKind();					// ��������״̬ѡ���Ӧͼ��
	QPointF getIconPos();					// ��ȡͼ�����λ��
	Time toPreciseTime(Time normalTime_);	// ���澫��ʱ��ת�����߾���ʱ��
	// ������һʱ��ͼ��λ��
	QPointF getIconDeltaPos(Time startTime_, Time runTime_, City* srcCity_, City* dstCity_);
private slots:
	void refreshProgressBar();				// ˢ�����н�����
};

#endif // !MAP_H