#ifndef CONTROL_H
#define CONTROL_H

#include "ProgramLib.h"

// ϵͳ������
class Control {
private:
	bool sysPause;								// ϵͳ״̬
	std::thread* sysClock;						// ϵͳ��ʱ���߳�
	std::thread* travelStatusRefresher;			// ϵͳˢ�����߳�
	void checkProcessExist() const;				// �ж��Ƿ����д˳���ʵ������
	void startLogSystem();						// ������־ϵͳ
	void startSysClock();						// ����ϵͳʱ��
	void incSysTime();							// ����ϵͳʱ��
	void initCityList();						// ��ʼ�������б�
	void initSchedule();						// ��ʼ��ʱ�̱�
	void startStatusRefresher();				// �����ÿ�״̬����ϵͳ
	void updateSystem() const;					// ����ϵͳ״̬
public:
	CityList cityList;							// �����б�
	Schedule schedule;							// ʱ�̱�
	InstanceList instanceList;					// ����ʵ���б�
	int instanceNum;							// ����ʵ������
	int cityNum;								// ���и���
	Time sysTimeNormal;							// ���澫��ϵͳʱ��
	Time sysTimePrecise;						// �߾���ϵͳʱ��
	std::ofstream* logFile;						// ��־�ļ������
	std::vector<std::string> vehKindStr;		// ��ͨ���������ַ�
	std::vector<std::string> dangerStr;			// ���з��������ַ�
	Control();									// ϵͳ��ʼ��
	~Control();									// ϵͳ�ر�
	bool addTravelInstance(InputData& input_);	// �������ʵ��
	City* IDtoCity(CityID id_);					// ���ݳ��б�Ż�ó���ָ��
	// ��ȡ�ƻ��������ڽ�ͨ���ߵȴ�ʱ��
	Time getWaitTime(Time lastVehArriveTime_, Time nextVehStartTime_) const;
	void pause();								// ϵͳ��ͣ
	void proceed();								// ϵͳ����
	bool isPaused() const;						// ��ȡϵͳ״̬
	void openSupport() const;					// ������֧��ҳ��
};

#endif // !CONTROL_H