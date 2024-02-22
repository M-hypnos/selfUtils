#include "utils/Any.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace selfUtils;
using namespace std;

struct Object {
    Object() = default;
    Object(int id, string name)
        : id_(id), name_(move(name))
    {
    }
    int id_{};
    string name_;
};

int main(){
    Any n;
    n = 2.2;
    printf("=====>>  %f\n", any_cast<double>(n));
    n = 1;
    printf("=====>>  %d\n", any_cast<int>(n));
    n = "xxxxx";
    printf("=====>>  %s\n", any_cast<char const*>(n));

    Any t("qqqqqqqqq");
    printf("=====>>  %s\n", any_cast<const char*>(t));

    string s1 = "hello";
    t = s1;
    cout << any_cast<string>(t) << endl;
    t = Object(323, string("eeeeee"));
    printf("======>>  %d\n", any_cast<Object>(t).id_);
    cout << any_cast<Object>(t).name_ << endl;

    Any tt(n);
    printf("====>>  type %s\n", tt.getType().name());
    printf("=====>>  %s\n", any_cast<char const*>(tt));
    Any a;
    a = n;
    printf("====>>  type %s\n", a.getType().name());
    printf("=====>>  %s\n", any_cast<char const*>(a));

    Any k = vector<int>{1,2,3};
    Any v;
    v = move(k);
    if (k.isNull()) {
        cout << "k isNull" << endl;
    }
    else {
        cout << "k isNotNull" << endl;
    }
    for (auto i : any_cast<vector<int>>(v)) {
        cout << i << endl;
    }
    Any q(move(v));
    if (v.isNull()) {
        cout << "v isNull" << endl;
    }
    else {
        cout << "v isNotNull" << endl;
    }
    for (auto i : any_cast<vector<int>>(q)) {
        cout << i << endl;
    }
    const Any vv = 1;
    q = 4;
    Any oo = any_cast<int>(vv) + any_cast<int>(q);
    printf("%d", any_cast<int>(oo));
    return 0;
}