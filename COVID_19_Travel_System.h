#ifndef COVID_19_TRAVEL_SYSTEM_H
#define COVID_19_TRAVEL_SYSTEM_H

#include "ui_COVID_19_Travel_System.h"
#include "ProgramLib.h"

// ����������
class COVID_19_Travel_System: public QMainWindow {
	Q_OBJECT
public:
	COVID_19_Travel_System(QWidget* parent = Q_NULLPTR);
	void callMainUIProceed();					// ��֪ϵͳʱ�Ӽ���
	int getCurInsIndex();						// ��ȡ UI ��ǰѡ������ÿͱ��
	Ui::COVID_19_Travel_SystemClass* getUI();	// ���������ڷ���������Ԫ��
private:
	Ui::COVID_19_Travel_SystemClass ui;
	void startUIRefresher();					// ���� UI ˢ�������߳�
	void refreshUITime();						// ˢ�� UI ��ʾʱ��
private slots:
	void refreshInfo();							// ˢ�� UI ��ǰѡ����ÿ���Ϣ
	void refreshUI();							// ˢ�� UI ��Ϣ
	void UIPause();								// ��֪���Ʋ���ͣʱ��
	void UIProceed();							// ��֪���Ʋ����ʱ��
	void setCustomizeBeginTimeStatus();			// �����Զ�����ʼʱ�����ѡ��������ʼʱ����������״̬
	void setLimitStatus();						// ������ʱ���ѡ��������ʱ��������״̬
	void onPressSubmit();						// ��Ӧ"�ύ������Ϣ"����¼�
	void onPressViewDetails();					// ��Ӧ"�鿴��ϸ��Ϣ"����¼�
	void onPressVehicleQuery();					// ��Ӧ"����/�г�/������ѯ"����¼�
	void onPressCityDangerQuery();				// ��Ӧ"���з��յȼ���ѯ"����¼�
	void onPressSupport();						// ��Ӧ"����֧��"����¼�
signals:
	void sendRefreshSignal();					// ���ͽ���ˢ���ź�
};

#endif // !COVID_19_TRAVEL_SYSTEM_H