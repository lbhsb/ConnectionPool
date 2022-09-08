#include <mysql.h>
#include <string>
#include <iostream>
#include "CommanConnectionPool.hpp"
using namespace std;

void testPool(int num)
{
	clock_t begin = clock();

	ConnectionPool* pool = ConnectionPool::getInstance();
	for (int i = 0; i < num; i++)
	{
		shared_ptr<Connection> conn = pool->getConnect();
		string sql("");
		sql = "insert into boys (boyName, userCP) values ('lbh', 120)";
		conn->update(sql);
	}

	clock_t end = clock();
	cout << num << " connectionPool use case cost " << end - begin << " ms" << endl;
}

void testNormal(int num)
{
	clock_t begin = clock();

	for (int i = 0; i < num; i++)
	{
		Connection conn;
		string sql("");
		sql = "insert into boys (boyName, userCP) values ('lbh', 119)";
		conn.connect("127.0.0.1", 3306, "root", "123", "girls");
		conn.update(sql);
	}

	clock_t end = clock();
	cout << num << " normal  use case cost " << end - begin << " ms" << endl;
}

int main()
{
	//thread t1(&testNormal, 100);
	thread t2(&testPool, 100);
	//t1.join();
	t2.join();
    return 0;
}
