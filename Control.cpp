#include "Control.h"
#include "DataStructure.h"
#include "GeneratePlan.h"

Control System;
PlanGenerator Algorithm;

// 初始化系统
Control::Control()
	: sysPause(false), sysClock(nullptr), travelStatusRefresher(nullptr),
	instanceNum(0), cityNum(0), sysTimeNormal(0), sysTimePrecise(0), logFile(nullptr) {
	// 若检查到已经有该程序的进程在系统运行，则阻止运行
	checkProcessExist();
	// 启动日志系统
	startLogSystem();
	// 启动系统时钟
	startSysClock();
	std::string line85(85, '=');
	std::string line20(20, '=');
	vehKindStr = {"", "飞机", "列车", "汽车"};
	dangerStr = {"", "低风险", "中风险", "高风险"};
	Debug << line85 << "\n系统启动\n" << line85 << std::endl;
	*logFile << line85 << "\n系统启动\n" << line85 << std::endl;
	// 初始化城市列表
	initCityList();
	Debug << line20 << std::endl;
	*logFile << line20 << std::endl;
	// 初始化时刻表
	initSchedule();
	Debug << line85 << std::endl;
	*logFile << line85 << std::endl;
	// 启动旅客状态刷新器
	startStatusRefresher();
}

// 关闭系统，释放资源
Control::~Control() {
	std::string line85(85, '=');
	// 释放城市列表
	for (auto city : cityList) {
		delete city;
	}
	// 释放时刻表
	for (auto veh : schedule) {
		delete veh;
	}
	// 释放旅客列表
	for (auto ins : instanceList) {
		delete ins;
	}
	// 关闭时钟线程
	delete sysClock;
	// 关闭旅客状态刷新器
	delete travelStatusRefresher;
	Debug << line85 << "\n系统正常退出\n" << line85 << std::endl;
	*logFile << line85 << "\n系统正常退出\n" << line85 << std::endl;
	logFile->close();
	// 关闭日志记录系统
	delete logFile;
}

//如果程序已经有一个在运行，则返回 true
void Control::checkProcessExist() const {
	HANDLE hMutex = CreateMutex(NULL, FALSE, L"DevState");
	if (hMutex && (GetLastError() == ERROR_ALREADY_EXISTS)) {
		CloseHandle(hMutex);
		hMutex = NULL;
		MessageBox(nullptr,
			L"另一个应用程序实例已在运行，不能同时运行多个应用程序实例。",
			L"程序已在运行 - 低风险旅行模拟系统", MB_OK);
		exit(0);
	}
}

// 启动日志记录系统
void Control::startLogSystem() {
	logFile = new std::ofstream;
	logFile->open("COVID_19_Travel_System_Log.log");
}

// 启动系统时钟线程
void Control::startSysClock() {
	sysClock = new std::thread(&Control::incSysTime, this);
	sysClock->detach();
}

// 刷新系统时间
void Control::incSysTime() {
	while (true) {
		// 以最高精度频率刷新时间
		Sleep(CLOCK_FREQ_PRECISE);
		// 系统不处于暂停状态下才进行刷新
		if (!sysPause) {
			// 高精度时间递增
			sysTimePrecise++;
			// 常规精度时间根据高精度时间换算
			sysTimeNormal = sysTimePrecise / (CLOCK_FREQ_NORMAL / CLOCK_FREQ_PRECISE);
		}
		// 防止溢出，高精度时间达到最大值时强制关闭程序
		if (sysTimePrecise == INT_MAX) {
			MessageBox(nullptr,
				L"程序单次运行时间已达上限，请重新启动应用程序。",
				L"程序运行时间截止 - 低风险旅行模拟系统", MB_OK);
			exit(0);
		}
	}
}

// 初始化城市列表
void Control::initCityList() {
	std::ifstream cityListFile;
	cityListFile.open("CityList");
	// 检查文件状态，若打开失败则弹出提示，终止程序
	if (!cityListFile.is_open()) {
		Debug << "初始化城市列表失败" << std::endl;
		*logFile << "初始化城市列表失败" << std::endl;
		MessageBox(nullptr,
			L"初始化程序时发生错误，未在该目录下找到城市列表数据文件\"CityList\"。\n\n即将跳转到程序支持页面。",
			L"初始化错误 - 低风险旅行模拟系统", MB_OK);
		// 指引用户在线下载数据文件
		openSupport();
		exit(-1);
	}
	// 打开成功则读取文件
	while (!cityListFile.eof()) {
		int intKind;
		CityID ID;
		CityName name;
		CityKind kind;
		int posX, posY;
		cityListFile >> ID >> name >> intKind >> posX >> posY;
		kind = static_cast<CityKind>(intKind);
		City* newCity = new City(ID, name, kind, posX, posY);
		cityList.push_back(newCity);
		cityNum++;
	}
	Debug << "初始化城市列表成功" << std::endl;
	*logFile << "初始化城市列表成功" << std::endl;
	cityListFile.close();
}

// 初始化时刻表
void Control::initSchedule() {
	std::ifstream scheduleFile;
	scheduleFile.open("Schedule");
	// 检查文件状态，若打开失败则弹出提示，终止程序
	if (!scheduleFile.is_open()) {
		Debug << "初始化时刻表失败" << std::endl;
		*logFile << "初始化时刻表失败" << std::endl;
		MessageBox(nullptr,
			L"初始化程序时发生错误，未在该目录下找到时刻表数据文件\"Schedule\"。\n\n即将跳转到程序支持页面。",
			L"初始化错误 - 低风险旅行模拟系统", MB_OK);
		// 指引用户在线下载数据文件
		openSupport();
		exit(-1);
	}
	// 打开成功则读取文件
	while (!scheduleFile.eof()) {
		int intKind;
		VehID ID{0};
		VehKind kind;
		VehName name;
		CityID srcID, dstID;
		Time startTime, runTime;
		scheduleFile >> srcID >> dstID >> intKind >> startTime >> runTime >> name;
		kind = static_cast<VehKind>(intKind);
		Vehicle* newVeh = new Vehicle(ID, kind, name, srcID, dstID, startTime, runTime);
		schedule.push_back(newVeh);
		ID++;
	}
	Debug << "初始化时刻表成功" << std::endl;
	*logFile << "初始化时刻表成功" << std::endl;
	scheduleFile.close();
}

// 旅客状态刷新器
void Control::startStatusRefresher() {
	travelStatusRefresher = new std::thread(&Control::updateSystem, this);
	travelStatusRefresher->detach();
}

// 刷新旅客状态
void Control::updateSystem() const {
	while (true) {
		Sleep(CLOCK_FREQ_NORMAL);
		// 系统运行状态才刷新
		if (!sysPause) {
			for (auto ins : instanceList) {
				if (System.sysTimeNormal <= ins->getBeginTime() + ins->getLastTime()) {
					Debug << "当前时间: " << System.sysTimeNormal << "\t旅行 ID: " << ins->getID() <<
						", 当前位置: " << ins->getStatusLog(System.sysTimeNormal) << std::endl;
					*logFile << "当前时间: " << System.sysTimeNormal << "\t旅行 ID: " << ins->getID() <<
						", 当前位置: " << ins->getStatusLog(System.sysTimeNormal) << std::endl;
				}
			}
		}
	}
}

// 添加旅行实例，成功返回 true，失败返回 false
bool Control::addTravelInstance(InputData& input_) {
	TravelID ID = instanceNum;
	Time lastTime{0};
	DangerIndex finalDangerIndex{0};
	TravelInstance* newInstance{nullptr};
	// 调用程序核心算法
	Plan* plan = Algorithm.generatePlan(input_.beginID, input_.endID,
		(input_.beginTime % 24), lastTime, finalDangerIndex, input_.limitTime);
	// 找到合适方案，则添加旅行实例到列表
	if (plan) {
		newInstance = new TravelInstance(ID, input_.request, input_.beginID, input_.endID,
			input_.beginTime, lastTime, finalDangerIndex, plan, input_.limitTime);
		instanceNum++;
		instanceList.push_back(newInstance);
		Debug << "当前时间: " << System.sysTimeNormal << "\t新建旅行实例: " << newInstance->toStringLog() << std::endl;
		*logFile << "当前时间: " << System.sysTimeNormal << "\t新建旅行实例: " << newInstance->toStringLog() << std::endl;
		return true;
	}
	// 无可用方案，反馈失败信息
	Debug << "当前时间: " << System.sysTimeNormal << "\t" << "尝试创建旅行: " <<
		System.IDtoCity(input_.beginID)->getName() << "->" << System.IDtoCity(input_.endID)->getName() <<
		", 出发时间: " << input_.beginTime << " 时" << ", 限时: " << input_.limitTime << " 小时" <<
		", 旅行限时过短，未找到合适的旅行方案" << std::endl;
	*logFile << "当前时间: " << System.sysTimeNormal << "\t" << "尝试创建旅行: " <<
		System.IDtoCity(input_.beginID)->getName() << "->" << System.IDtoCity(input_.endID)->getName() <<
		", 出发时间: " << input_.beginTime << " 时" << ", 限时: " << input_.limitTime << " 小时" <<
		", 旅行限时过短，未找到合适的旅行方案" << std::endl;
	return false;
}

// 根据城市编号获取城市指针
City* Control::IDtoCity(CityID id_) {
	CityList::iterator iter = System.cityList.begin();
	if (id_ < cityList.size()) {
		return *(iter + id_);
	}
	return nullptr;
}

// 获取在某一城市的等待时间
Time Control::getWaitTime(Time lastVehArriveTime_, Time nextVehStartTime_) const {
	// 不跨天，直接相减
	if (lastVehArriveTime_ <= nextVehStartTime_) {
		return nextVehStartTime_ - lastVehArriveTime_;
	}
	// 跨天，进行模运算
	return nextVehStartTime_ - lastVehArriveTime_ + 24;
}

// 系统暂停
void Control::pause() {
	sysPause = true;
}

// 系统继续
void Control::proceed() {
	sysPause = false;
}

// 获取系统状态，处于暂停返回 true，否则返回 false
bool Control::isPaused() const {
	return sysPause == true;
}

// 打开在线支持链接
void Control::openSupport() const {
	ShellExecute(NULL, L"open", L"explorer.exe",
		L"http://www.myzhang.site/data-structure-course-design-support", NULL, SW_SHOW);
}