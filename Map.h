#ifndef MAP_H
#define MAP_H

#include "ProgramLib.h"

// 实时地图模块
class Map: public QWidget {
	Q_OBJECT
public:
	Map(QWidget* parent = Q_NULLPTR);
	~Map() = default;
private:
	QTimer* rePaintSignal;					// 地图刷新定时器
	Status curInsStatus;					// 当前旅行实例的状态
	void paintEvent(QPaintEvent* event);	// 绘图事件
	QPixmap getIconKind();					// 根据旅行状态选择对应图标
	QPointF getIconPos();					// 获取图标绘制位置
	Time toPreciseTime(Time normalTime_);	// 常规精度时间转换到高精度时间
	// 计算下一时刻图标位移
	QPointF getIconDeltaPos(Time startTime_, Time runTime_, City* srcCity_, City* dstCity_);
private slots:
	void refreshProgressBar();				// 刷新旅行进度条
};

#endif // !MAP_H