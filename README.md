# STDB
一个轻量化的时序数据库，基于rocksdb进行封装

# 构建方法
- 首先按照[这里的说明](https://github.com/facebook/rocksdb)编译rocksdb
- 然后在自己的工程中加入STDB.h和STDB.cpp两个文件

# API说明
#### 打开或新建数据库
STDB(const char* file_path)
- 构造函数，打开指定目录对应的数据文件，如果不存在则创建

#### 保存数据记录
##### int set(const string &key, long long timestamp, const string &val)
- 保存一条记录
- key 为名称主键，通常为通道接口名称，双通道接口注意区分in和out
- timestamp 为时间戳
- val 为要保存的具体数据，是二进制安全的

##### seti(const string &key, long long timestamp, const int val)
- 保存整数值，常用于保存模拟量采集卡数据

##### setb(const string &key, long long timestamp, const bool val)
- 保存布尔值，常用于保存信号量量采集数据

#### 读取数据记录

##### 读取指定时间戳的单条记录
- int get(const string &key, long long timestamp, string *val)
- int geti(const string &key, long long timestamp, int *val)
- int getb(const string &key, long long timestamp, bool *val)

##### 读取指定时间戳开始的多条记录
- int lget(const std::string &key, long long begin, int limit, vector<pair<long long, string>> *ret)
- int lgeti(const std::string &key, long long begin, int limit, vector<pair<long long, int>> *ret)
- int lgetb(const std::string &key, long long begin, int limit, vector<pair<long long, bool>> *ret)
- 说明：limit为要读取的记录数，可以为负数，表示最新的limit条数据

#### 读取指定时间戳范围内的多条记录
- int lrange(const std::string &key, long long begin, long long end, vector<pair<long long, string>> *ret, long long step=0)
- int lrangei(const std::string &key, long long begin, long long end, vector<pair<long long, int>> *ret, long long step=0)
- int lrangeb(const std::string &key, long long begin, long long end, vector<pair<long long, bool>> *ret, long long step=0)
- 说明1：读取的范围为时间戳位于[begin, end) 之间的记录
- 说明2: step用于指定时间戳步长，0表示连续读取；大于0的整数表示间隔step读取一个条记录
