// ����mysql�������Լ���ɾ�Ĳ�

#include <mysql.h>
#include <ctime>
#include <string>
using namespace std;

class Connection
{
private:
    MYSQL *conn_;

    // ���ʱ��
    clock_t aliveTime;
public:
    Connection();
    ~Connection();
    
    // ���ݿ������
    bool connect(string ip, unsigned int port, string user, string password, string dbname);

    // ��ɾ�Ķ�����update
    bool update(string sql);

    // ��ѯ
    MYSQL_RES *query(string sql);

    // ��ÿ��������ʱ�����
    void setTime();
    clock_t getTime();
};