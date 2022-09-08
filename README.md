# 在C++基础上实现mysql连接池

## 项目背景
常见的用户通过网络通信与服务器连接对数据库进行操作涉及到TCP/IP中的“三次握手”，“四次挥手”等会造成大量性能消耗
因此，受内存池的启发，**为减少连接造成的性能消耗，增加连接池**。

## 涉及知识点
- 基于C/C++的mysql数据库编程
- 多线程编程
  - 线程互斥与同步
  - 原子操作
  - 生产者消费者模型
- 设计模式

## 功能介绍
***
1. 初始连接：初始化时创建一个连接池维护一个initSize大小的连接**于一个队列中**，
当应用发起MySQL访问时，不用创建新的MySQL Server连接，而是从连接池中直接获取一个连接，当使用完成后，再把连接归还到连接池中。
***
2. 最大连接：当并发访问MySQL Server的请求增加，初始连接量不够用了，此时会增加连接量，但是增加的连接量有个上限就是maxSIze。
因为每一个连接都会占用一个socket资源，一般连接池和服务器都是部署在一台主机上，如果连接池的连接数量过多，那么服务器就不能响应太多的客户端请求了。
***
3. 最大空闲时间：高并发而新创建的连接在很长时间（maxIdleTime）内没有得到使用，那么这些新创建的连接处于空闲，并且占用着一定的资源。
这个时候就需要将其释放掉，最终只用保存iniSize个连接就行。
***
4. 连接超时：
当MySQL的并发访问请求量过大，连接池中的连接数量已经达到了maxSize，并且此时连接池中没有可以使用的连接，那么此时应用阻塞connectionTimeOut的时间。
如果此时间内有使用完的连接归还到连接池，那么他就可以使用，如果超过这个时间还是没有连接，那么它获取数据库连接池失败，无法访问数据库。
***
## 压力测试
- 修改server.ini文件中的配置
- 以及main.cpp中的test函数sql语句
