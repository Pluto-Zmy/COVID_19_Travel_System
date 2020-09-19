#include "Map.h"
#include "COVID_19_Travel_System.h"
#include "Control.h"
#include "DataStructure.h"

extern Control System;
extern COVID_19_Travel_System* MainWindow;

// 初始化地图模块
Map::Map(QWidget* parent)
	: QWidget(parent), rePaintSignal(nullptr), curInsStatus(WAIT) {
	this->setAutoFillBackground(true);
	// 导入地图图片
	QPalette palette = this->palette();
	palette.setBrush(QPalette::Background, QBrush(QPixmap(":/COVID_19_Travel_System/Resources/Map.png")));
	this->setPalette(palette);
	// 开启用于绘制地图的高精度计时器
	rePaintSignal = new QTimer;
	rePaintSignal->start(1000 / 60);
	connect(rePaintSignal, SIGNAL(timeout()), this, SLOT(update()));
	connect(rePaintSignal, &QTimer::timeout, this, &Map::refreshProgressBar);
}

// 绘图事件，以刷新率绘图
void Map::paintEvent(QPaintEvent* event) {
	// 设置画笔样式
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	QPen pen;
	pen.setWidth(3);
	pen.setColor(QColor(219, 121, 122));
	pen.setStyle(Qt::DashLine);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	// 旅行列表非空时，则绘制当前选择的旅行
	if (!System.instanceList.empty()) {
		TravelInstance* curIns = *(System.instanceList.begin() + MainWindow->getCurInsIndex());
		// 根据途径城市绘制城市之间的连线
		for (auto veh : *(curIns->getPlan())) {
			painter.setPen(pen);
			painter.drawLine(veh->getSrcCity()->getLinePos(), veh->getDstCity()->getLinePos());
		}
		// 根据旅客状态和位置绘制图标
		painter.drawPixmap(getIconPos(), getIconKind());
	}
}

// 根据旅客状态选择对应图标
QPixmap Map::getIconKind() {
	switch (curInsStatus) {
		case HANG:		return QPixmap(":/COVID_19_Travel_System/Resources/Hang.png");
		case WAIT:		return QPixmap(":/COVID_19_Travel_System/Resources/Wait.png");
		case RUN_AIR:	return QPixmap(":/COVID_19_Travel_System/Resources/Airplane.png");
		case RUN_TRN:	return QPixmap(":/COVID_19_Travel_System/Resources/Train.png");
		case RUN_BUS:	return QPixmap(":/COVID_19_Travel_System/Resources/Bus.png");
		case FINISHED:	return QPixmap(":/COVID_19_Travel_System/Resources/Finished.png");
		default:		return QPixmap();
	}
}

// 核心模块，确定高精度系统时间下旅客的实时位置
QPointF Map::getIconPos() {
	TravelInstance* curIns = *(System.instanceList.begin() + MainWindow->getCurInsIndex());
	// 绘图需要利用旅行信息中的关键时间节点序列
	const std::vector<Time>* const curKeyTime = curIns->getKeyTime();
	QPointF curIconPos;
	// 系统时间小于旅行开始时间，表示旅行尚未开始
	if (System.sysTimePrecise <= toPreciseTime(curIns->getBeginTime())) {
		curIconPos = curIns->getBeginCity()->getIconPos();
		curInsStatus = HANG;
	}
	// 系统时间大于旅行结束时间，表示旅行已完成
	else if (System.sysTimePrecise >= toPreciseTime(curIns->getBeginTime() + curIns->getLastTime())) {
		curIconPos = curIns->getEndCity()->getIconPos();
		curInsStatus = FINISHED;
	}
	else {
		for (int i{0}; i < curKeyTime->size(); i++) {
			Vehicle* curVeh = *(curIns->getPlan()->begin() + i / 2);
			if (System.sysTimePrecise < toPreciseTime(*(curKeyTime->begin() + i))) {
				// 时间节点序号为偶数，表示正在候车
				if (0 == i % 2) {
					curIconPos = curVeh->getSrcCity()->getIconPos();
					curInsStatus = WAIT;
				}
				// 时间节点序号为奇数，表示正在乘车
				else {
					curIconPos = curVeh->getSrcCity()->getIconPos();
					// 运动中则需要计算坐标增量
					curIconPos += getIconDeltaPos(*(curKeyTime->begin() + i - 1), curVeh->getRunTime(),
						curVeh->getSrcCity(), curVeh->getDstCity());
					// 根据交通工具类型选择对应图标
					curInsStatus = static_cast<Status>(curVeh->getKind());
				}
				break;
			}
		}
	}
	return curIconPos;
}

// 常规精度时间转换为高精度时间
Time Map::toPreciseTime(Time normalTime_) {
	return normalTime_ * (CLOCK_FREQ_NORMAL / CLOCK_FREQ_PRECISE);
}

// 根据当前车次信息计算下一高精度时刻旅客位置的变化量
QPointF Map::getIconDeltaPos(Time startTime_, Time runTime_, City* srcCity_, City* dstCity_) {
	// 坐标变换比率，为"该车次已用时间 ÷ 车程"
	double deltaRatio = (static_cast<double>(System.sysTimePrecise) - static_cast<double>(toPreciseTime(startTime_)))
		/ static_cast<double>(toPreciseTime(runTime_));
	// 坐标变化量，为"两地直线距离 × 坐标变换比率"
	double deltaX = (dstCity_->getIconPos() - srcCity_->getIconPos()).x() * deltaRatio;
	double deltaY = (dstCity_->getIconPos() - srcCity_->getIconPos()).y() * deltaRatio;
	return QPointF(deltaX, deltaY);
}

// 刷新进度条
void Map::refreshProgressBar() {
	// 旅客列表非空时才刷新
	if (!System.instanceList.empty()) {
		TravelInstance* curIns = *(System.instanceList.begin() + MainWindow->getCurInsIndex());
		// 设置进度条取值范围，此处为旅行持续高精度时间
		MainWindow->getUI()->progressBar->setRange(0, toPreciseTime(curIns->getLastTime()));
		MainWindow->getUI()->progressBar->setMinimum(0);
		MainWindow->getUI()->progressBar->setMaximum(toPreciseTime(curIns->getLastTime()));
		// 旅行开始后，结束前才显示并刷新进度条；否则隐藏
		if (System.sysTimePrecise >= toPreciseTime(curIns->getBeginTime()) &&
			System.sysTimePrecise <= toPreciseTime(curIns->getBeginTime() + curIns->getLastTime())) {
			MainWindow->getUI()->progressBar->setVisible(true);
			MainWindow->getUI()->progressBar->setValue(System.sysTimePrecise - toPreciseTime(curIns->getBeginTime()));
		}
		else {
			MainWindow->getUI()->progressBar->setVisible(false);
		}
	}
}