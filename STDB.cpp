
#include "STDB.h"
#include <iostream>
#include <sstream> 
using namespace std;

STDB::STDB(const char* file_path) {
    Options options;
    options.create_if_missing = true;
    auto status = DB::Open(options, file_path, &_db);
    if(!status.ok()) {
        std::cout << status.ToString() << std::endl;
    }
}

STDB::~STDB() {
    delete _db;
}

int STDB::set(const string &key, long long timestamp, const string &val) {
    char buff[22];
    sprintf(buff, "|s%019lld", timestamp);
    string keys = key + buff;
    auto status = _db->Put(WriteOptions(), keys, val);
    if(!status.ok()) {
        std::cout << status.ToString() << std::endl;
    }
    return STDB_OK;
}

int STDB::seti(const string &key, long long timestamp, const int val) {
    char buff[22];
    sprintf(buff, "|i%019lld", timestamp);
    string keyi = key + buff;
    string sval = to_string(val);
    auto status = _db->Put(WriteOptions(), keyi, sval);
    if(!status.ok()) {
        std::cout << status.ToString() << std::endl;
    }
    return STDB_OK;
}

int STDB::setb(const string &key, long long timestamp, const bool val) {
    char buff[22];
    sprintf(buff, "|b%019lld", timestamp);
    string keyb = key + buff;
    auto status = _db->Put(WriteOptions(), keyb, val ? "T" : "F");
    if(!status.ok()) {
        std::cout << status.ToString() << std::endl;
    }
    return STDB_OK;
}

int STDB::get(const string &key, long long timestamp, string *val) {
    if(val==NULL) {
        return STDB_ERR_NIL;
    }
    char buff[22];
    sprintf(buff, "|s%019lld", timestamp);
    string keys = key + buff;
    auto status = _db->Get(ReadOptions(), keys, val);
    if(status.ok()) {
        return STDB_OK;
    }
    return status.IsNotFound() ? STDB_ERR_NOTFOUND : STDB_ERR_NIL;
}

int STDB::geti(const string &key, long long timestamp, int *val) {
    if(val==NULL) {
        return STDB_ERR_NIL;
    }

    char buff[22];
    sprintf(buff, "|i%019lld", timestamp);
    string keyi = key + buff;
    string siv;
    auto status = _db->Get(ReadOptions(), keyi, &siv);
    if(status.ok()) {
        stringstream ss;
        ss << siv;
        ss >> *val;
        return STDB_OK;
    }
    return status.IsNotFound() ? STDB_ERR_NOTFOUND : STDB_ERR_NIL;
}

int STDB::getb(const string &key, long long timestamp, bool *val) {
    if(val==NULL) {
        return STDB_ERR_NIL;
    }

    char buff[22];
    sprintf(buff, "|b%019lld", timestamp);
    string keyb = key + buff;
    string sbv;
    auto status = _db->Get(ReadOptions(), keyb, &sbv);
    if(status.ok()) {
        *val = (sbv == "T");
        return STDB_OK;
    }
    return status.IsNotFound() ? STDB_ERR_NOTFOUND : STDB_ERR_NIL;
}

int STDB::lget(const std::string &key, long long begin, int limit, vector<pair<long long, string>> *ret) {
    if(ret==NULL) {
        return STDB_ERR_NIL;
    }

    Iterator* it = _db->NewIterator(ReadOptions());
    char buff[22];
    int count = 0;

    if(limit<=0) {
        limit = limit==0 ? 1 : -limit;
        begin = 9223372036854775807;

        sprintf(buff, "|s%019lld", begin);
        string key_from = key + buff;
        string key_pre = key + "|s";

        for(it->SeekForPrev(key_from); it->Valid() && it->key().starts_with(key_pre); it->Prev()) {
            auto tm = atoll(it->key().data() + (it->key().size()-19));
            if(tm<begin) {
                break;
            }
            ret->insert(ret->begin(), pair<long long, string>(tm, string(it->value().data(), it->value().size())));
            if(++count == limit) {
                break;
            }
        }
    } else {
        sprintf(buff, "|s%019lld", begin);
        string key_from = key + buff;
        string key_pre = key + "|s";

        for(it->Seek(key_from); it->Valid() && it->key().starts_with(key_pre); it->Next()) {
            auto tm = atoll(it->key().data() + (it->key().size()-19));
            ret->push_back(pair<long long, string>(tm, string(it->value().data(), it->value().size())));
            if(++count == limit) {
                break;
            }
        }
    }

    delete it;
    return STDB_OK;
}


int STDB::lgeti(const std::string &key, long long begin, int limit, vector<pair<long long, int>> *ret) {
    if(ret==NULL) {
        return STDB_ERR_NIL;
    }

    Iterator* it = _db->NewIterator(ReadOptions());
    char buff[22];
    int count = 0;

    if(limit<=0) {
        limit = limit==0 ? 1 : -limit;
        begin = 9223372036854775807;

        sprintf(buff, "|i%019lld", begin);
        string key_from = key + buff;
        string key_pre = key + "|i";

        for(it->SeekForPrev(key_from); it->Valid() && it->key().starts_with(key_pre); it->Prev()) {
            auto tm = atoll(it->key().data() + (it->key().size()-19));
            if(tm<begin) {
                break;
            }
            stringstream ss;
            int v;
            ss << string(it->value().data(), it->value().size());
            ss >> v;
            ret->insert(ret->begin(), pair<long long, int>(tm, v));
            if(++count == limit) {
                break;
            }
        }
        
    } else {
        sprintf(buff, "|i%019lld", begin);
        string key_from = key + buff;
        string key_pre = key + "|i";
        
        for(it->Seek(key_from); it->Valid() && it->key().starts_with(key_pre); it->Next()) {
            auto tm = atoll(it->key().data() + (it->key().size()-19));
            stringstream ss;
            int v;
            ss << string(it->value().data(), it->value().size());
            ss >> v;
            ret->push_back(pair<long long, int>(tm, v));
            if(++count == limit) {
                break;
            }
        }
    }
    
    delete it;
    return STDB_OK;
}

int STDB::lgetb(const std::string &key, long long begin, int limit, vector<pair<long long, bool>> *ret) {
    if(ret==NULL) {
        return STDB_ERR_NIL;
    }

    Iterator* it = _db->NewIterator(ReadOptions());
    char buff[22];
    int count = 0;

    if(limit<=0) {
        limit = limit==0 ? 1 : -limit;
        begin = 9223372036854775807;

        sprintf(buff, "|b%019lld", begin);
        string key_from = key + buff;
        string key_pre = key + "|b";

        for(it->SeekForPrev(key_from); it->Valid() && it->key().starts_with(key_pre); it->Prev()) {
            auto tm = atoll(it->key().data() + (it->key().size()-19));
            if(tm<begin) {
                break;
            }
            ret->insert(ret->begin(), pair<long long, bool>(tm, it->value().data()[0]=='T'));
            if(++count == limit) {
                break;
            }
        }
    } else {
        sprintf(buff, "|b%019lld", begin);
        string key_from = key + buff;
        string key_pre = key + "|b";
        for(it->Seek(key_from); it->Valid() && it->key().starts_with(key_pre); it->Next()) {
            auto tm = atoll(it->key().data() + (it->key().size()-19));
            ret->push_back(pair<long long, bool>(tm, it->value().data()[0]=='T'));
            if(++count == limit) {
                break;
            }
        } 
    }
    
    delete it;
    return STDB_OK;
}

int STDB::lrange(const std::string &key, long long begin, long long end, vector<pair<long long, string>> *ret, long long step) {
    if(ret==NULL || end<begin) {
        return STDB_ERR_NIL;
    }

    char buff[22];
    sprintf(buff, "|s%019lld", begin);
    string key_from = key + buff;
    string key_pre = key + "|s";

    Iterator* it = _db->NewIterator(ReadOptions());

    if(step>0) {
        long long next_step = begin;
        bool recorded = false;
        for(it->Seek(key_from); it->Valid() && it->key().starts_with(key_pre); it->Next()) {
            auto tm = atoll(it->key().data() + (it->key().size()-19));
            if(tm>=end) {
                break;
            }
            if(tm<=next_step) {
                if(recorded) {
                    continue;
                }
                ret->push_back(pair<long long, string>(tm, string(it->value().data(), it->value().size())));
                recorded = true;
            } else {
                ret->push_back(pair<long long, string>(tm, string(it->value().data(), it->value().size())));
                while (next_step<=tm) {
                    next_step += step;
                }
                recorded = false;
            }
        }
    } else {
        for(it->Seek(key_from); it->Valid() && it->key().starts_with(key_pre); it->Next()) {
            auto tm = atoll(it->key().data() + (it->key().size()-19));
            if(tm>end) {
                break;
            }
            ret->push_back(pair<long long, string>(tm, string(it->value().data(), it->value().size())));
        }
    }
    
    delete it;
    return STDB_OK;
}

int STDB::lrangei(const std::string &key, long long begin, long long end, vector<pair<long long, int>> *ret, long long step) {
    if(ret==NULL || end<begin) {
        return STDB_ERR_NIL;
    }

    char buff[22];
    sprintf(buff, "|i%019lld", begin);
    string key_from = key + buff;
    string key_pre = key + "|i";

    Iterator* it = _db->NewIterator(ReadOptions());

    if(step>0) {
        long long next_step = begin;
        bool recorded = false;
        for(it->Seek(key_from); it->Valid() && it->key().starts_with(key_pre); it->Next()) {
            auto tm = atoll(it->key().data() + (it->key().size()-19));
            if(tm>=end) {
                break;
            }
            if(tm<=next_step) {
                if(recorded) {
                    continue;
                }
                stringstream ss;
                int v;
                ss << string(it->value().data(), it->value().size());
                ss >> v;
                ret->push_back(pair<long long, int>(tm, v));
                recorded = true;
            } else {
                stringstream ss;
                int v;
                ss << string(it->value().data(), it->value().size());
                ss >> v;
                ret->push_back(pair<long long, int>(tm, v));
                while (next_step<=tm) {
                    next_step += step;
                }
                recorded = false;
            }
        }
    } else {
        for(it->Seek(key_from); it->Valid() && it->key().starts_with(key_pre); it->Next()) {
            auto tm = atoll(it->key().data() + (it->key().size()-19));
            if(tm>=end) {
                break;
            }
            stringstream ss;
            int v;
            ss << string(it->value().data(), it->value().size());
            ss >> v;
            ret->push_back(pair<long long, int>(tm, v));
        }        
    }
    
    delete it;
    return STDB_OK;
}

int STDB::lrangeb(const std::string &key, long long begin, long long end, vector<pair<long long, bool>> *ret, long long step) {
    if(ret==NULL || end<begin) {
        return STDB_ERR_NIL;
    }

    char buff[22];
    sprintf(buff, "|b%019lld", begin);
    string key_from = key + buff;
    string key_pre = key + "|b";

    Iterator* it = _db->NewIterator(ReadOptions());
    
    if(step>0) {
        long long next_step = begin;
        bool recorded = false;
        for(it->Seek(key_from); it->Valid() && it->key().starts_with(key_pre); it->Next()) {
            auto tm = atoll(it->key().data() + (it->key().size()-19));
            if(tm>=end) {
                break;
            }
            if(tm<=next_step) {
                if(recorded) {
                    continue;
                }
                ret->push_back(pair<long long, bool>(tm, it->value().data()[0]=='T'));
                recorded = true;
            } else {
                ret->push_back(pair<long long, bool>(tm, it->value().data()[0]=='T'));
                while (next_step<=tm) {
                    next_step += step;
                }
                recorded = false;
            }
        }
    } else {
        for(it->Seek(key_from); it->Valid() && it->key().starts_with(key_pre); it->Next()) {
            auto tm = atoll(it->key().data() + (it->key().size()-19));
            if(tm>=end) {
                break;
            }
            ret->push_back(pair<long long, bool>(tm, it->value().data()[0]=='T'));
        }        
    }

    
    delete it;
    return STDB_OK;
}
