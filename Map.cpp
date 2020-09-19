#include "Map.h"
#include "COVID_19_Travel_System.h"
#include "Control.h"
#include "DataStructure.h"

extern Control System;
extern COVID_19_Travel_System* MainWindow;

// ��ʼ����ͼģ��
Map::Map(QWidget* parent)
	: QWidget(parent), rePaintSignal(nullptr), curInsStatus(WAIT) {
	this->setAutoFillBackground(true);
	// �����ͼͼƬ
	QPalette palette = this->palette();
	palette.setBrush(QPalette::Background, QBrush(QPixmap(":/COVID_19_Travel_System/Resources/Map.png")));
	this->setPalette(palette);
	// �������ڻ��Ƶ�ͼ�ĸ߾��ȼ�ʱ��
	rePaintSignal = new QTimer;
	rePaintSignal->start(1000 / 60);
	connect(rePaintSignal, SIGNAL(timeout()), this, SLOT(update()));
	connect(rePaintSignal, &QTimer::timeout, this, &Map::refreshProgressBar);
}

// ��ͼ�¼�����ˢ���ʻ�ͼ
void Map::paintEvent(QPaintEvent* event) {
	// ���û�����ʽ
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	QPen pen;
	pen.setWidth(3);
	pen.setColor(QColor(219, 121, 122));
	pen.setStyle(Qt::DashLine);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	// �����б�ǿ�ʱ������Ƶ�ǰѡ�������
	if (!System.instanceList.empty()) {
		TravelInstance* curIns = *(System.instanceList.begin() + MainWindow->getCurInsIndex());
		// ����;�����л��Ƴ���֮�������
		for (auto veh : *(curIns->getPlan())) {
			painter.setPen(pen);
			painter.drawLine(veh->getSrcCity()->getLinePos(), veh->getDstCity()->getLinePos());
		}
		// �����ÿ�״̬��λ�û���ͼ��
		painter.drawPixmap(getIconPos(), getIconKind());
	}
}

// �����ÿ�״̬ѡ���Ӧͼ��
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

// ����ģ�飬ȷ���߾���ϵͳʱ�����ÿ͵�ʵʱλ��
QPointF Map::getIconPos() {
	TravelInstance* curIns = *(System.instanceList.begin() + MainWindow->getCurInsIndex());
	// ��ͼ��Ҫ����������Ϣ�еĹؼ�ʱ��ڵ�����
	const std::vector<Time>* const curKeyTime = curIns->getKeyTime();
	QPointF curIconPos;
	// ϵͳʱ��С�����п�ʼʱ�䣬��ʾ������δ��ʼ
	if (System.sysTimePrecise <= toPreciseTime(curIns->getBeginTime())) {
		curIconPos = curIns->getBeginCity()->getIconPos();
		curInsStatus = HANG;
	}
	// ϵͳʱ��������н���ʱ�䣬��ʾ���������
	else if (System.sysTimePrecise >= toPreciseTime(curIns->getBeginTime() + curIns->getLastTime())) {
		curIconPos = curIns->getEndCity()->getIconPos();
		curInsStatus = FINISHED;
	}
	else {
		for (int i{0}; i < curKeyTime->size(); i++) {
			Vehicle* curVeh = *(curIns->getPlan()->begin() + i / 2);
			if (System.sysTimePrecise < toPreciseTime(*(curKeyTime->begin() + i))) {
				// ʱ��ڵ����Ϊż������ʾ���ں�
				if (0 == i % 2) {
					curIconPos = curVeh->getSrcCity()->getIconPos();
					curInsStatus = WAIT;
				}
				// ʱ��ڵ����Ϊ��������ʾ���ڳ˳�
				else {
					curIconPos = curVeh->getSrcCity()->getIconPos();
					// �˶�������Ҫ������������
					curIconPos += getIconDeltaPos(*(curKeyTime->begin() + i - 1), curVeh->getRunTime(),
						curVeh->getSrcCity(), curVeh->getDstCity());
					// ���ݽ�ͨ��������ѡ���Ӧͼ��
					curInsStatus = static_cast<Status>(curVeh->getKind());
				}
				break;
			}
		}
	}
	return curIconPos;
}

// ���澫��ʱ��ת��Ϊ�߾���ʱ��
Time Map::toPreciseTime(Time normalTime_) {
	return normalTime_ * (CLOCK_FREQ_NORMAL / CLOCK_FREQ_PRECISE);
}

// ���ݵ�ǰ������Ϣ������һ�߾���ʱ���ÿ�λ�õı仯��
QPointF Map::getIconDeltaPos(Time startTime_, Time runTime_, City* srcCity_, City* dstCity_) {
	// ����任���ʣ�Ϊ"�ó�������ʱ�� �� ����"
	double deltaRatio = (static_cast<double>(System.sysTimePrecise) - static_cast<double>(toPreciseTime(startTime_)))
		/ static_cast<double>(toPreciseTime(runTime_));
	// ����仯����Ϊ"����ֱ�߾��� �� ����任����"
	double deltaX = (dstCity_->getIconPos() - srcCity_->getIconPos()).x() * deltaRatio;
	double deltaY = (dstCity_->getIconPos() - srcCity_->getIconPos()).y() * deltaRatio;
	return QPointF(deltaX, deltaY);
}

// ˢ�½�����
void Map::refreshProgressBar() {
	// �ÿ��б�ǿ�ʱ��ˢ��
	if (!System.instanceList.empty()) {
		TravelInstance* curIns = *(System.instanceList.begin() + MainWindow->getCurInsIndex());
		// ���ý�����ȡֵ��Χ���˴�Ϊ���г����߾���ʱ��
		MainWindow->getUI()->progressBar->setRange(0, toPreciseTime(curIns->getLastTime()));
		MainWindow->getUI()->progressBar->setMinimum(0);
		MainWindow->getUI()->progressBar->setMaximum(toPreciseTime(curIns->getLastTime()));
		// ���п�ʼ�󣬽���ǰ����ʾ��ˢ�½���������������
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