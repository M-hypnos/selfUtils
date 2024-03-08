// utils.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vld.h>
#include <iostream>
#include "utils/Any.h"
#include "utils/SmartPtr.h"
#include <string>
#include <assert.h>
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

struct Doo
{
    Doo() { cout << "Doo ctor\n"; }
    Doo(const Doo&) { cout << "Doo copy ctor\n"; }
    Doo(Doo&&) { cout << "Doo move ctor\n"; }
    ~Doo() { cout << "~Doo dtor\n"; }
    Doo(int v) : val(v) { cout << "Doo ctor\n"; }
    int val;
};

void legacy_api(Doo* owning_foo)
{
    std::cout << __func__ << '\n';
    delete owning_foo;
}

void AnyTest() {
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

    Any k = vector<int>{ 1,2,3 };
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

void UniquePtrTest() {
    cout << "Example constructor(1)...\n";
    UniquePtr<Doo> up1a;
    UniquePtr<Doo> up1b(nullptr);
    cout << "Example constructor(2)...\n";
    {
        UniquePtr<Doo> up2(new Doo());
    }
    cout << "Example constructor(3)...\n";
    {
        UniquePtr<Doo> up3a(new Doo);
        UniquePtr<Doo> up3b(move(up3a));
    }
    cout << "Example constructor(4)...\n";
    {
        UniquePtr<Doo> up4a(new Doo);
        UniquePtr<Doo> up4b = move(up4a);
    }

    UniquePtr<Doo> managed_foo(new Doo);
    legacy_api(managed_foo.release());

    assert(managed_foo.get() == nullptr);

    UniquePtr<Doo> up(new Doo());
    cout << "up  " << up.get() << '\n';
    up.reset(new Doo());
    cout << "up  " << up.get() << '\n';
    up.reset();
    if (up) {
        cout << "up not nullptr \n";
    }
    else {
        cout << "up is nullptr \n";
    }

    UniquePtr<Doo> up1(new Doo(1));
    UniquePtr<Doo> up2(new Doo(2));
    cout << "up1->val:" << up1->val << "  up2->val:" << up2->val << '\n';
    up1.swap(up2);
    cout << "up1->val:" << up1->val << "  up2->val:" << up2->val << '\n';

    UniquePtr<Doo> upA(new Doo);
    UniquePtr<Doo> upB(new Doo);
    upB = move(upA);
}

WeakPtr<int> gwp;

void observe_gwp()
{
    cout << "use_count(): " << gwp.use_count() << "\t id: ";
    if (auto sp = gwp.lock())
        std::cout << *sp << '\n';
    else
        std::cout << "??\n";
}

void share_recursively(SharedPtr<int> sp, int depth)
{
    observe_gwp();
    if (1 < depth)
        share_recursively(sp, depth - 1);
    observe_gwp();
}

WeakPtr<int> gw;
void f()
{
    if (!gw.expired())
        cout << "gw is valid\n";
    else
        cout << "gw is expired\n";
}

void observe(WeakPtr<int> weak)
{
    if (auto p = weak.lock())
        cout << "\tobserve() is able to lock weak_ptr<>, value=" << *p << '\n';
    else
        cout << "\tobserve() is unable to lock weak_ptr<>\n";
}

void WeakPtrTest() {
    WeakPtr<Foo> w_ptr;
    {
        auto ptr = SharedPtr<Foo>(new Foo);
        w_ptr = ptr;
        cout << "w_ptr.use_count() inside scope: " << w_ptr.use_count() << '\n';
    }
    cout << "w_ptr.use_count() out of scope: " << w_ptr.use_count() << '\n';
    cout << "w_ptr.expired() out of scope: " << boolalpha << w_ptr.expired() << '\n';
    cout<< '\n';
    //---------------
    auto shared = SharedPtr<int>(new int), shared2 = shared, shared3 = shared2;
    auto weak = WeakPtr<int>{ shared };
    cout << boolalpha
        << "shared.use_count(): " << shared.use_count() << '\n'
        << "weak.use_count(): " << weak.use_count() << '\n'
        << "weak.expired(): " << weak.expired() << '\n';

    weak.reset();
    cout << "weak.reset();\n"
        << "shared.use_count(): " << shared.use_count() << '\n'
        << "weak.use_count(): " << weak.use_count() << '\n'
        << "weak.expired(): " << weak.expired() << '\n';
    cout << '\n';
    //---------------
    SharedPtr<Foo> sp1 = SharedPtr<Foo>(new Foo(100));
    SharedPtr<Foo> sp2 = SharedPtr<Foo>(new Foo(200));
    WeakPtr<Foo> wp1 = sp1;
    WeakPtr<Foo> wp2 = sp2;
    auto print = [&]()
    {
        auto p1 = wp1.lock();
        auto p2 = wp2.lock();
        cout << " p1=" << (p1 ? p1->print() : "nullptr");
        cout << " p2=" << (p2 ? p2->print() : "nullptr") << '\n';
    };
    print();
    wp1.swap(wp2);
    print();
    wp1.reset();
    print();
    wp1.swap(wp2);
    print();
    cout << '\n';
    //---------------
    observe_gwp();
    {
        auto sp = SharedPtr<int>(new int(42));
        gwp = sp;
        observe_gwp();
        share_recursively(sp, 3);
        observe_gwp();
    }
    observe_gwp();
    cout << '\n';
    //---------------
    {
        auto sp = SharedPtr<int>(new int(42));
        gw = sp;
        f();
    }
    f();
    cout << '\n';
    //---------------
    WeakPtr<int> weakP;
    cout << "weak_ptr<> is not yet initialized\n";
    observe(weakP);
    {
        auto shared = SharedPtr<int>(new int(42));
        weakP = shared;
        cout << "weak_ptr<> is initialized with shared_ptr\n";
        observe(weakP);
    }
    cout << "shared_ptr<> has been destructed due to scope exit\n";
    observe(weakP);
}

class Parent {
public:
    Parent() {
        printf("Parent ctor\n");
    }
    virtual ~Parent() {
        printf("~Parent destroy\n");
    }
    void normalTest() {
        printf("Parent normalTest \n");
    }
    virtual void virtualTest() {
        printf("Parent virtualTest \n");
    }
};

class Child: public Parent {
public:
    Child() {
        printf("Child ctor\n");
    }
    virtual ~Child() {
        printf("~Child destroy\n");
    }
    void normalTest() {
        printf("Child normalTest \n");
    }
    virtual void virtualTest() {
        printf("Child virtualTest \n");
    }
};

void SmartPtrTest() {
    cout << "-----" << '\n';
    SharedPtr<int> sp1(new int(10));
    WeakPtr<int> wp1 = sp1;
    cout << sp1.use_count() << '\n';
    wp1.lock();
    cout << sp1.use_count() << '\n';
    SharedPtr<int> sp2 = wp1.lock();
    cout << sp1.use_count() << '\n';
    cout << sp2.use_count() << '\n';
    cout << "-----" << '\n';
    SharedPtr<int> sptr1;
    cout << sptr1.use_count() << '\n';
    SharedPtr<int> sptr2 = sptr1;
    cout << sptr2.use_count() << '\n';
    SharedPtr<int> sptr3(new int(10));
    sptr1 = sptr3;
    cout << sptr1.use_count() << '\n';
    cout << sptr2.use_count() << '\n';
    cout << sptr3.use_count() << '\n';

    {
        SharedPtr<Parent> p1(new Child());
        p1->normalTest();
        p1->virtualTest();

        SharedPtr<Child> c1 = new Child();
        SharedPtr<Parent> p2 = c1;
        p2->normalTest();
        p2->virtualTest();
        cout << "p2: " << p2.use_count() << '\n';
        SharedPtr<Parent> p3 = SharedPtr<Child>(new Child());
        p3->normalTest();
        p3->virtualTest();
        cout << "p3: " << p3.use_count() << '\n';
        p1 = p3;
        cout<< "c1: " << c1.use_count() << '\n';
        p2 = p3;
        cout<< "c1: " << c1.use_count() << '\n';
        cout << "p3: " << p3.use_count() << '\n';
    }
}

int main() {
    AnyTest();
    SharedPtrTest();
    UniquePtrTest();
    WeakPtrTest();
    SmartPtrTest();
    return 0;
}