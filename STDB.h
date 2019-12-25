/*
** STDB.h for SCH in /home/pi/sch/src/db
**
** Made by soldiest
** Login   <solidest>
**
** Started on  Tue Dec 10 1:49:41 PM 2019 soldiest
** Last update Thu Dec 25 11:08:24 AM 2019 solidest
*/

#ifndef STDB_H_
# define STDB_H_

#ifndef ROCKSDB_LITE
#define ROCKSDB_LITE
#endif

#include <string>
#include <vector>
#include <map>

#include <rocksdb/db.h>
#include <rocksdb/merge_operator.h>
#include <rocksdb/slice.h>
#include <rocksdb/write_batch.h>

using namespace rocksdb;
using namespace std;

#define STDB_OK              0
#define STDB_ERR_NOTFOUND   -1
#define STDB_ERR_NIL        -2

class STDB
{
    private:
        DB* _db;


    public:
        STDB(const char* file_path);
        ~STDB();

	    int set(const string &key, long long timestamp, const string &val);
	    int seti(const string &key, long long timestamp, const int val);
	    int setb(const string &key, long long timestamp, const bool val);

        int get(const string &key, long long timestamp, string *val);
        int geti(const string &key, long long timestamp, int *val);
        int getb(const string &key, long long timestamp, bool *val);

        int lget(const std::string &key, long long begin, int limit, vector<pair<long long, string>> *ret);
        int lgeti(const std::string &key, long long begin, int limit, vector<pair<long long, int>> *ret);
        int lgetb(const std::string &key, long long begin, int limit, vector<pair<long long, bool>> *ret);

        int lrange(const std::string &key, long long begin, long long end, vector<pair<long long, string>> *ret, long long step=0);
        int lrangei(const std::string &key, long long begin, long long end, vector<pair<long long, int>> *ret, long long step=0);
        int lrangeb(const std::string &key, long long begin, long long end, vector<pair<long long, bool>> *ret, long long step=0);

};

#endif /* !STDB_H_ */
