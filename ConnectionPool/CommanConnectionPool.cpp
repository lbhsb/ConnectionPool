// 连接池文件
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

	// 初始化连接池，建立initsize个连接
	for (int i = 0; i < initSize; i++)
	{
		Connection* con = new Connection();
		cout << ip << port <<user << password<<  dbname << endl;
		con->connect(ip, port, user, password, dbname);
		con->setTime();
		connectQueue.push(con);
		numConnect++;
	}

	// 开启生产者线程和清理线程
	thread produceConnect(&ConnectionPool::produce, this);
	thread releaseFreeConnect(&ConnectionPool::release, this);
	produceConnect.detach();
	releaseFreeConnect.detach();
}

// 产生连接,入队
void ConnectionPool::produce()
{
	while (1)
	{
		unique_lock<mutex> lck(queueMutex);

		// 等待用户取连接，直到没有连接且连接数小于最大数才开始生产连接
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

// 取出连接，出队
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
	* 自定义删除器，连接取出
	* 析构后不删除，而是加入队头, 注意加入队头也需要锁
	* 注意此时匿名函数的语法，必须是这么写
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

// 释放空闲的连接
void ConnectionPool::release()
{
	while (1)
	{
		// 每隔timealive时间进行一次检查
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
		// 按行读取
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
* 测试函数，用于进行压力测试,具体sql语句根据自己创建的数据库来修改
* 参数：sql语句个数
*/
