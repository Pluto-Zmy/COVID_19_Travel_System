#ifndef PROGRAM_LIB_H
#define PROGRAM_LIB_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <windows.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QStyleFactory>
#include <QMessagebox>
#include <QWidget>
#include <QDebug>
#include <QPainter>
#include <QTimer>

#define Debug std::cout

// 前向引用声明
class City;
class Vehicle;
class TravelInstance;
class InputData;
class Control;

// 定义数据类型别名
typedef int CityID;
typedef int VehID;
typedef int TravelID;
typedef int Time;
typedef int DangerIndex;
typedef std::string CityName;
typedef std::string VehName;
typedef std::vector<City*> CityList;
typedef std::vector<Vehicle*> Schedule, Plan;
typedef std::vector<TravelInstance*> InstanceList;

// 定义枚举
typedef enum {
	LOW = 1, MID = 2, HIGH = 3
} CityKind;
typedef enum {
	AIRPLANE = 1, TRAIN = 2, BUS = 3
} VehKind;
typedef enum {
	NOLIMIT = 0, LIMIT = 1
} Request, PlanKind;
typedef enum {
	HANG = -1, WAIT = 0, RUN_AIR = 1, RUN_TRN = 2, RUN_BUS = 3, FINISHED = 4
} Status;

// 定义常量
constexpr Time INS_TIME_MAX = 240;
constexpr DangerIndex DANGER_MAX = INT_MAX;
constexpr int CLOCK_FREQ_NORMAL = 1e4;
constexpr int CLOCK_FREQ_MEDIUM = 1e3;
constexpr int CLOCK_FREQ_PRECISE = 1e2;

#endif // !PROGRAM_LIB_H