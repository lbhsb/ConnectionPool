// 数据库的增删改查
#include "public.hpp"
#include "Connection.hpp"


Connection::Connection()
{
    // 创建对象的api
    conn_ = mysql_init(nullptr);
}

Connection::~Connection()
{
    if (!conn_)
    {
        mysql_close(conn_);
    }
}

bool Connection::connect(string ip, unsigned int port, string user, string password, string dbname)
{
    // 连接的api
    MYSQL *p = mysql_real_connect(conn_, ip.c_str(), user.c_str(), password.c_str(), dbname.c_str(), port, NULL, 0);
    if (p == nullptr)
    {
        LOG("connect failed");
        return false;
    }
    return true;
}

bool Connection::update(string sql)
{
    if (mysql_query(conn_, sql.c_str()))
    {
        LOG("update error:" + sql);
        cout << "error num is" << mysql_errno(conn_) << endl;
        return false;
    }
    return true;
}

MYSQL_RES *Connection::query(string sql)
{
    if (mysql_query(conn_, sql.c_str()))
    {
        cout << mysql_query(conn_,sql.c_str()) << endl;
        return nullptr;
    }

    // 查询结果的api
    return mysql_use_result(conn_);
}

void Connection::setTime()
{
    aliveTime = clock();
}

clock_t Connection::getTime()
{
    return clock() - aliveTime;
}