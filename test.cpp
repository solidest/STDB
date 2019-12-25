

#include <iostream>
#include <pthread.h>
#include <fstream>
#include <string.h>
#include <uv.h>

#include "STDB.h"
using namespace std;



int main(int argcs, char ** argvs) {

    STDB db("/tmp/test.stdb");

    for(int i=0; i<1000; i++) {
        db.set("interface_s_in", i, "{a:100, b:200, x:1000.89222},{a:100, b:200, x:1000.89222},{a:100, b:200, x:1000.89222},{a:100, b:200, x:1000.89222},{a:100, b:200, x:1000.89222}");
        db.seti("interface_i", i, i);
        db.setb("interface_b", i, i%2==0);           
    }

    string result1;
    db.get("interface_s_in", 189, &result1);
    cout << result1 << endl;


    vector<pair<long long, int>> result2;
    db.lrangei("interface_i", 8, 10, &result2);
    for(int i=0; i<result2.size(); i++) {
        cout << result2[i].first << " : " << result2[i].second << endl;
    }


    vector<pair<long long, bool>> result3;
    db.lgetb("interface_b", 189, 10, &result3);
    for(int i=0; i<result3.size(); i++) {
        cout << result3[i].first << " : " << result3[i].second << endl;
    }
    return 0;
}

