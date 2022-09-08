// ���ӳ��ļ�
#include"CommanConnectionPool.hpp"
#include<thread>
#include<iostream>
#include"public.hpp"
#define LINE_SIZE 1024

ConnectionPool* ConnectionPool::getInstance()
{
	static ConnectionPool pool;
	return &pool;
}

ConnectionPool::ConnectionPool()
{
	if (!loadConfig())
	{
		LOG("load config failed");
		return;
	}

	// ��ʼ�����ӳأ�����initsize������
	for (int i = 0; i < initSize; i++)
	{
		Connection* con = new Connection();
		cout << ip << port <<user << password<<  dbname << endl;
		con->connect(ip, port, user, password, dbname);
		con->setTime();
		connectQueue.push(con);
		numConnect++;
	}

	// �����������̺߳������߳�
	thread produceConnect(&ConnectionPool::produce, this);
	thread releaseFreeConnect(&ConnectionPool::release, this);
	produceConnect.detach();
	releaseFreeConnect.detach();
}

// ��������,���
void ConnectionPool::produce()
{
	while (1)
	{
		unique_lock<mutex> lck(queueMutex);

		// �ȴ��û�ȡ���ӣ�ֱ��û��������������С��������ſ�ʼ��������
		while (!connectQueue.empty() && numConnect < maxSize)
		{
			cv.wait(lck);
		}

		Connection* con = new Connection();
		con->connect(ip, port, user, password, dbname);
		con->setTime();
		connectQueue.push(con);
		numConnect++;

		cv.notify_all();
	}
}

// ȡ�����ӣ�����
shared_ptr<Connection> ConnectionPool::getConnect()
{
	unique_lock<mutex> lck(queueMutex);
	while (connectQueue.empty())
	{
		cv.wait_for(lck, chrono::milliseconds(timeOut));
		if (connectQueue.empty())
		{
			LOG("get connection failed, time out");
			return nullptr;
		}
	}

	/*
	* �Զ���ɾ����������ȡ��
	* ������ɾ�������Ǽ����ͷ, ע������ͷҲ��Ҫ��
	* ע���ʱ�����������﷨����������ôд
	*/
	shared_ptr<Connection> spc(connectQueue.front(), [&](Connection* con)
		{
			unique_lock<mutex> lck(queueMutex);
			con->setTime();
			connectQueue.push(con);
		});
	connectQueue.pop();
	
	cv.notify_all();
	return spc;
}

// �ͷſ��е�����
void ConnectionPool::release()
{
	while (1)
	{
		// ÿ��timealiveʱ�����һ�μ��
		this_thread::sleep_for(chrono::seconds(timeIdle));

		unique_lock<mutex> lck(queueMutex);
		while (numConnect > initSize)
		{
			Connection* con = connectQueue.front();
			if (con->getTime() > timeIdle * 1000)
			{
				connectQueue.pop();
				delete con;
				numConnect--;
			}
		}
	}
}

bool ConnectionPool::loadConfig()
{
	FILE* f = nullptr;
	fopen_s(&f, "./server.ini", "r");
	if (f == nullptr)
	{
		LOG("load config failed");
		return false;
	}

	while (!feof(f))
	{
		// ���ж�ȡ
		char line[LINE_SIZE] = "";
		fgets(line, LINE_SIZE, f);
		string str(line);
		int index = str.find('=', 0);
		if (index == -1) continue;

		int endIndex = str.find('\n', index);
		string key = str.substr(0, index);
		string value = str.substr(index + 1, endIndex - index - 1);

		if (key == "ip") ip = value;
		if (key == "port") port = atoi(value.c_str());
		if (key == "password") password = value;
		if (key == "username") user = value;
		if (key == "dbname") dbname = value;
		if (key == "initSize") initSize = atoi(value.c_str());
		if (key == "maxSize") maxSize = atoi(value.c_str());
		if (key == "maxIdleTime") timeIdle = atoi(value.c_str());
		if (key == "connectionTimeout") timeOut = atoi(value.c_str());
	}
	return true;	
}

/*
* ���Ժ��������ڽ���ѹ������,����sql�������Լ����������ݿ����޸�
* ������sql������
*/
