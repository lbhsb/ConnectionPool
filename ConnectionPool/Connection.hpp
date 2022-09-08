// 用于mysql的连接以及增删改查

#include <mysql.h>
#include <ctime>
#include <string>
using namespace std;

class Connection
{
private:
    MYSQL *conn_;

    // 存活时间
    clock_t aliveTime;
public:
    Connection();
    ~Connection();
    
    // 数据库的连接
    bool connect(string ip, unsigned int port, string user, string password, string dbname);

    // 增删改都属于update
    bool update(string sql);

    // 查询
    MYSQL_RES *query(string sql);

    // 对每个连接做时间控制
    void setTime();
    clock_t getTime();
};