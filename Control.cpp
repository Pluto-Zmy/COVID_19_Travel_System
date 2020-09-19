#include "Control.h"
#include "DataStructure.h"
#include "GeneratePlan.h"

Control System;
PlanGenerator Algorithm;

// ��ʼ��ϵͳ
Control::Control()
	: sysPause(false), sysClock(nullptr), travelStatusRefresher(nullptr),
	instanceNum(0), cityNum(0), sysTimeNormal(0), sysTimePrecise(0), logFile(nullptr) {
	// ����鵽�Ѿ��иó���Ľ�����ϵͳ���У�����ֹ����
	checkProcessExist();
	// ������־ϵͳ
	startLogSystem();
	// ����ϵͳʱ��
	startSysClock();
	std::string line85(85, '=');
	std::string line20(20, '=');
	vehKindStr = {"", "�ɻ�", "�г�", "����"};
	dangerStr = {"", "�ͷ���", "�з���", "�߷���"};
	Debug << line85 << "\nϵͳ����\n" << line85 << std::endl;
	*logFile << line85 << "\nϵͳ����\n" << line85 << std::endl;
	// ��ʼ�������б�
	initCityList();
	Debug << line20 << std::endl;
	*logFile << line20 << std::endl;
	// ��ʼ��ʱ�̱�
	initSchedule();
	Debug << line85 << std::endl;
	*logFile << line85 << std::endl;
	// �����ÿ�״̬ˢ����
	startStatusRefresher();
}

// �ر�ϵͳ���ͷ���Դ
Control::~Control() {
	std::string line85(85, '=');
	// �ͷų����б�
	for (auto city : cityList) {
		delete city;
	}
	// �ͷ�ʱ�̱�
	for (auto veh : schedule) {
		delete veh;
	}
	// �ͷ��ÿ��б�
	for (auto ins : instanceList) {
		delete ins;
	}
	// �ر�ʱ���߳�
	delete sysClock;
	// �ر��ÿ�״̬ˢ����
	delete travelStatusRefresher;
	Debug << line85 << "\nϵͳ�����˳�\n" << line85 << std::endl;
	*logFile << line85 << "\nϵͳ�����˳�\n" << line85 << std::endl;
	logFile->close();
	// �ر���־��¼ϵͳ
	delete logFile;
}

//��������Ѿ���һ�������У��򷵻� true
void Control::checkProcessExist() const {
	HANDLE hMutex = CreateMutex(NULL, FALSE, L"DevState");
	if (hMutex && (GetLastError() == ERROR_ALREADY_EXISTS)) {
		CloseHandle(hMutex);
		hMutex = NULL;
		MessageBox(nullptr,
			L"��һ��Ӧ�ó���ʵ���������У�����ͬʱ���ж��Ӧ�ó���ʵ����",
			L"������������ - �ͷ�������ģ��ϵͳ", MB_OK);
		exit(0);
	}
}

// ������־��¼ϵͳ
void Control::startLogSystem() {
	logFile = new std::ofstream;
	logFile->open("COVID_19_Travel_System_Log.log");
}

// ����ϵͳʱ���߳�
void Control::startSysClock() {
	sysClock = new std::thread(&Control::incSysTime, this);
	sysClock->detach();
}

// ˢ��ϵͳʱ��
void Control::incSysTime() {
	while (true) {
		// ����߾���Ƶ��ˢ��ʱ��
		Sleep(CLOCK_FREQ_PRECISE);
		// ϵͳ��������ͣ״̬�²Ž���ˢ��
		if (!sysPause) {
			// �߾���ʱ�����
			sysTimePrecise++;
			// ���澫��ʱ����ݸ߾���ʱ�任��
			sysTimeNormal = sysTimePrecise / (CLOCK_FREQ_NORMAL / CLOCK_FREQ_PRECISE);
		}
		// ��ֹ������߾���ʱ��ﵽ���ֵʱǿ�ƹرճ���
		if (sysTimePrecise == INT_MAX) {
			MessageBox(nullptr,
				L"���򵥴�����ʱ���Ѵ����ޣ�����������Ӧ�ó���",
				L"��������ʱ���ֹ - �ͷ�������ģ��ϵͳ", MB_OK);
			exit(0);
		}
	}
}

// ��ʼ�������б�
void Control::initCityList() {
	std::ifstream cityListFile;
	cityListFile.open("CityList");
	// ����ļ�״̬������ʧ���򵯳���ʾ����ֹ����
	if (!cityListFile.is_open()) {
		Debug << "��ʼ�������б�ʧ��" << std::endl;
		*logFile << "��ʼ�������б�ʧ��" << std::endl;
		MessageBox(nullptr,
			L"��ʼ������ʱ��������δ�ڸ�Ŀ¼���ҵ������б������ļ�\"CityList\"��\n\n������ת������֧��ҳ�档",
			L"��ʼ������ - �ͷ�������ģ��ϵͳ", MB_OK);
		// ָ���û��������������ļ�
		openSupport();
		exit(-1);
	}
	// �򿪳ɹ����ȡ�ļ�
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
	Debug << "��ʼ�������б�ɹ�" << std::endl;
	*logFile << "��ʼ�������б�ɹ�" << std::endl;
	cityListFile.close();
}

// ��ʼ��ʱ�̱�
void Control::initSchedule() {
	std::ifstream scheduleFile;
	scheduleFile.open("Schedule");
	// ����ļ�״̬������ʧ���򵯳���ʾ����ֹ����
	if (!scheduleFile.is_open()) {
		Debug << "��ʼ��ʱ�̱�ʧ��" << std::endl;
		*logFile << "��ʼ��ʱ�̱�ʧ��" << std::endl;
		MessageBox(nullptr,
			L"��ʼ������ʱ��������δ�ڸ�Ŀ¼���ҵ�ʱ�̱������ļ�\"Schedule\"��\n\n������ת������֧��ҳ�档",
			L"��ʼ������ - �ͷ�������ģ��ϵͳ", MB_OK);
		// ָ���û��������������ļ�
		openSupport();
		exit(-1);
	}
	// �򿪳ɹ����ȡ�ļ�
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
	Debug << "��ʼ��ʱ�̱�ɹ�" << std::endl;
	*logFile << "��ʼ��ʱ�̱�ɹ�" << std::endl;
	scheduleFile.close();
}

// �ÿ�״̬ˢ����
void Control::startStatusRefresher() {
	travelStatusRefresher = new std::thread(&Control::updateSystem, this);
	travelStatusRefresher->detach();
}

// ˢ���ÿ�״̬
void Control::updateSystem() const {
	while (true) {
		Sleep(CLOCK_FREQ_NORMAL);
		// ϵͳ����״̬��ˢ��
		if (!sysPause) {
			for (auto ins : instanceList) {
				if (System.sysTimeNormal <= ins->getBeginTime() + ins->getLastTime()) {
					Debug << "��ǰʱ��: " << System.sysTimeNormal << "\t���� ID: " << ins->getID() <<
						", ��ǰλ��: " << ins->getStatusLog(System.sysTimeNormal) << std::endl;
					*logFile << "��ǰʱ��: " << System.sysTimeNormal << "\t���� ID: " << ins->getID() <<
						", ��ǰλ��: " << ins->getStatusLog(System.sysTimeNormal) << std::endl;
				}
			}
		}
	}
}

// �������ʵ�����ɹ����� true��ʧ�ܷ��� false
bool Control::addTravelInstance(InputData& input_) {
	TravelID ID = instanceNum;
	Time lastTime{0};
	DangerIndex finalDangerIndex{0};
	TravelInstance* newInstance{nullptr};
	// ���ó�������㷨
	Plan* plan = Algorithm.generatePlan(input_.beginID, input_.endID,
		(input_.beginTime % 24), lastTime, finalDangerIndex, input_.limitTime);
	// �ҵ����ʷ��������������ʵ�����б�
	if (plan) {
		newInstance = new TravelInstance(ID, input_.request, input_.beginID, input_.endID,
			input_.beginTime, lastTime, finalDangerIndex, plan, input_.limitTime);
		instanceNum++;
		instanceList.push_back(newInstance);
		Debug << "��ǰʱ��: " << System.sysTimeNormal << "\t�½�����ʵ��: " << newInstance->toStringLog() << std::endl;
		*logFile << "��ǰʱ��: " << System.sysTimeNormal << "\t�½�����ʵ��: " << newInstance->toStringLog() << std::endl;
		return true;
	}
	// �޿��÷���������ʧ����Ϣ
	Debug << "��ǰʱ��: " << System.sysTimeNormal << "\t" << "���Դ�������: " <<
		System.IDtoCity(input_.beginID)->getName() << "->" << System.IDtoCity(input_.endID)->getName() <<
		", ����ʱ��: " << input_.beginTime << " ʱ" << ", ��ʱ: " << input_.limitTime << " Сʱ" <<
		", ������ʱ���̣�δ�ҵ����ʵ����з���" << std::endl;
	*logFile << "��ǰʱ��: " << System.sysTimeNormal << "\t" << "���Դ�������: " <<
		System.IDtoCity(input_.beginID)->getName() << "->" << System.IDtoCity(input_.endID)->getName() <<
		", ����ʱ��: " << input_.beginTime << " ʱ" << ", ��ʱ: " << input_.limitTime << " Сʱ" <<
		", ������ʱ���̣�δ�ҵ����ʵ����з���" << std::endl;
	return false;
}

// ���ݳ��б�Ż�ȡ����ָ��
City* Control::IDtoCity(CityID id_) {
	CityList::iterator iter = System.cityList.begin();
	if (id_ < cityList.size()) {
		return *(iter + id_);
	}
	return nullptr;
}

// ��ȡ��ĳһ���еĵȴ�ʱ��
Time Control::getWaitTime(Time lastVehArriveTime_, Time nextVehStartTime_) const {
	// �����죬ֱ�����
	if (lastVehArriveTime_ <= nextVehStartTime_) {
		return nextVehStartTime_ - lastVehArriveTime_;
	}
	// ���죬����ģ����
	return nextVehStartTime_ - lastVehArriveTime_ + 24;
}

// ϵͳ��ͣ
void Control::pause() {
	sysPause = true;
}

// ϵͳ����
void Control::proceed() {
	sysPause = false;
}

// ��ȡϵͳ״̬��������ͣ���� true�����򷵻� false
bool Control::isPaused() const {
	return sysPause == true;
}

// ������֧������
void Control::openSupport() const {
	ShellExecute(NULL, L"open", L"explorer.exe",
		L"http://www.myzhang.site/data-structure-course-design-support", NULL, SW_SHOW);
}