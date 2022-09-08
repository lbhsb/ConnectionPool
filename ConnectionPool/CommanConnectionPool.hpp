/*
	作用：定义线程池
	具体模式：生产者消费者模型+单例模式+多线程
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
	static ConnectionPool* getInstance(); // 获取实例
	shared_ptr<Connection> getConnect(); // 获取连接
private:
	// 构造函数私有化(单例模式)
	ConnectionPool();

	// 生产者，生成一个连接
	void produce();

	// 释放连接
	void release();

	// 读取配置文件
	bool loadConfig();
private:
	// 维护一个队列，连接从队尾取，从队头出
	queue<Connection*> connectQueue;
	
	// 线程池初始化参数
	int initSize;
	int maxSize;
	// 获取连接超时时间
	int timeOut;
	// 连接池最大空闲时间
	int timeIdle;

	// 连接所需信息
	string ip;
	string user;
	string password;
	string dbname;
	unsigned int port;
	
	// 维护队列的线程安全
	mutex queueMutex;

	// 用于控制生产者消费者的互斥
	condition_variable cv;

	// 用于控制线程池的连接数量
	atomic_int numConnect;
};