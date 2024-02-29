#include "utils/Any.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace selfUtils;
using namespace std;

struct Object {
     Object(string s) : name_(s)
        {
            printf("object ctor: %s \n", name_.c_str());
        }
    Object(int id, string name)
        : id_(id), name_(move(name))
        {
        }
    ~Object()
        {
            printf("object delete: %s \n", name_.c_str());
        }
    int id_{};
    string name_;
};

struct Foo
    {
        Foo(int n = 0) noexcept : bar(n)
        {
            cout << "Foo::Foo(), bar = " << bar << " @ " << this << '\n';
        }
        ~Foo()
        {
            cout << "Foo::~Foo(), bar = " << bar << " @ " << this << '\n';
        }
        int getBar() const noexcept { return bar; }
        string print() const { return to_string(bar); }
    private:
        int bar;
    };

void AnyTest(){
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
}

void SharedPtrTest() {
    SharedPtr<Object> emptyPtr;
    printf("emptyPtr use count %d \n", emptyPtr.use_count());
    SharedPtr<Object> ptr(new Object("a"));
    printf("ptr use count %d \n", ptr.use_count());

    SharedPtr<Object> copyPtr(ptr);
    printf("ptr use count %d \n", ptr.use_count());
    printf("copyPtr use count %d \n", copyPtr.use_count());

    SharedPtr<Object> ptr1(new Object("b"));
    ptr1 = ptr1;
    printf("ptr1 use count %d \n", ptr1.use_count());
    SharedPtr<Object> copyPtr1(ptr1);
    ptr1 = ptr;
    printf("ptr use count %d \n", ptr.use_count());
    printf("ptr1 use count %d \n", ptr1.use_count());
    printf("copyPtr1 use count %d \n", copyPtr1.use_count());

    SharedPtr<Object> movePtr(move(ptr));
    printf("ptr use count %d \n", ptr.use_count());
    printf("movePtr use count %d \n", movePtr.use_count());

    SharedPtr<Object> p = ptr1;
    printf("ptr1 use count %d \n", ptr1.use_count());
    printf("p use count %d \n", p.use_count());

    SharedPtr<Object> moveAPtr(new Object("c"));
    SharedPtr<Object> ptr2 = moveAPtr;
    printf("ptr2 use count %d \n", ptr2.use_count());
    printf("moveAPtr use count %d \n", moveAPtr.use_count());

    moveAPtr = std::move(ptr1);
    printf("ptr1 use count %d \n", ptr1.use_count());
    printf("moveAPtr use count %d \n", moveAPtr.use_count());
    printf("p use count %d \n", p.use_count());
    printf("ptr2 use count %d \n", ptr2.use_count());
//------------------------------------------------
    cout << "1) unique ownership\n";
    {
        SharedPtr<Foo> sptr = new Foo(100);

        cout << "Foo::bar = " << sptr->getBar() << ", use_count() = "
            << sptr.use_count() << '\n';

        cout << "call sptr.reset()...\n";
        sptr.reset();
        cout << "After reset(): use_count() = " << sptr.use_count()
            << ", sptr = " << sptr.get() << '\n';
    }

    cout << "\n2) unique ownership\n";
    {
        SharedPtr<Foo> sptr = new Foo(200);

        cout << "Foo::bar = " << sptr->getBar() << ", use_count() = "
            << sptr.use_count() << '\n';

        cout << "call sptr.reset()...\n";
        sptr.reset(new Foo{ 222 });
        cout << "After reset(): use_count() = " << sptr.use_count()
            << ", sptr = " << sptr.get() << "\nLeaving the scope...\n";
    }

    cout << "\n3) multiple ownership\n";
    {
        SharedPtr<Foo> sptr1 = new Foo(300);
        SharedPtr<Foo> sptr2 = sptr1;
        SharedPtr<Foo> sptr3 = sptr2;

        cout << "Foo::bar = " << sptr1->getBar() << ", use_count() = "
            << sptr1.use_count() << '\n';

        cout << "call sptr1.reset()...\n";
        sptr1.reset(new Foo{ 333 });

        cout << "After reset():\n"
            << "sptr1.use_count() = " << sptr1.use_count()
            << ", sptr1 @ " << sptr1.get() << '\n'
            << "sptr2.use_count() = " << sptr2.use_count()
            << ", sptr2 @ " << sptr2.get() << '\n'
            << "sptr3.use_count() = " << sptr3.use_count()
            << ", sptr3 @ " << sptr3.get() << '\n'
            << "Leaving the scope...\n";
    }
//------------------------------------------------
    SharedPtr<Foo> p1 = new Foo(100);
    SharedPtr<Foo> p2 = new Foo(200);
    auto print = [&]()
    {
        cout << " p1=" << (p1.get() ? p1->print() : "nullptr");
        cout << " p2=" << (p2.get() ? p2->print() : "nullptr") << '\n';
    };
    print();

    p1.swap(p2);
    print();

    p1.reset();
    print();

    p1.swap(p2);
    print();
}

int main(){
    AnyTest();
    SharedPtrTest();
    return 0;
}