/*
	���ã������̳߳�
	����ģʽ��������������ģ��+����ģʽ+���߳�
*/ 
#include<queue>
#include"Connection.hpp"
#include<mutex>
#include<condition_variable>
#include<atomic>
#include<memory>
using namespace std;

class ConnectionPool
{
public:
	static ConnectionPool* getInstance(); // ��ȡʵ��
	shared_ptr<Connection> getConnect(); // ��ȡ����
private:
	// ���캯��˽�л�(����ģʽ)
	ConnectionPool();

	// �����ߣ�����һ������
	void produce();

	// �ͷ�����
	void release();

	// ��ȡ�����ļ�
	bool loadConfig();
private:
	// ά��һ�����У����ӴӶ�βȡ���Ӷ�ͷ��
	queue<Connection*> connectQueue;
	
	// �̳߳س�ʼ������
	int initSize;
	int maxSize;
	// ��ȡ���ӳ�ʱʱ��
	int timeOut;
	// ���ӳ�������ʱ��
	int timeIdle;

	// ����������Ϣ
	string ip;
	string user;
	string password;
	string dbname;
	unsigned int port;
	
	// ά�����е��̰߳�ȫ
	mutex queueMutex;

	// ���ڿ��������������ߵĻ���
	condition_variable cv;

	// ���ڿ����̳߳ص���������
	atomic_int numConnect;
};