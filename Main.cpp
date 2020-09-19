#include "COVID_19_Travel_System.h"

COVID_19_Travel_System* MainWindow{nullptr};

int main(int argc, char* argv[]) {
	QApplication App(argc, argv);
	QApplication::setStyle(QStyleFactory::create("Fusion"));
	QFont UIFont{"Microsoft YaHei UI"};
	UIFont.setPixelSize(12);
	App.setFont(UIFont);
	MainWindow = new COVID_19_Travel_System;
	MainWindow->show();
	return App.exec();
}