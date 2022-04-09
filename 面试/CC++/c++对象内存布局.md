**对象内存布局的影响**

一个类可能有如下的影响因素：

+ 成员变量
+ 虚函数（产生虚函数表）
+ 单一继承（只继承于一个类）
+ 多重继承（继承多个类）

+ 重复继承（继承的多个父类中其父类有相同的超类）

+ 虚拟继承（使用*virtual*方式继承，为了保证继承后父类的内存布局只会存在一份）

可以通过对象的地址来取得虚函数表的地址，如：

```c++
typedef void(*func)(void);
Base b;
std::cout << "虚函数表地址：" << (int*)(&b) << std::endl;
std::cout << "虚函数表——第一个函数地址：" << (int*)(*(int*)(&b)) << std::endl;

Fun pFun = NULL;
// Invoke the first virtual function 
pFun = (Fun)*((int*)*(int*)(&b)); 
pFun();
```

**单一继承**

假设有如下继承：

![img](https://p-blog.csdn.net/images/p_blog_csdn_net/haoel/EntryImages/20081015/dd01.jpg)

父类、子类、孙子类都有自己的一个成员变量，而子类覆盖了父类的f()方法，孙子类覆盖了子类的g_child()和超类的f()。源程序如下：

```c++
class Parent {
public:
    int iparent;
    Parent ():iparent (10) {}
    virtual void f() { cout << " Parent::f()" << endl; }
    virtual void g() { cout << " Parent::g()" << endl; }
    virtual void h() { cout << " Parent::h()" << endl; }
};

class Child : public Parent {
public:
    int ichild;
    Child():ichild(100) {}
    virtual void f() { cout << "Child::f()" << endl; }
    virtual void g_child() { cout << "Child::g_child()" << endl; }
    virtual void h_child() { cout << "Child::h_child()" << endl; }
};

class GrandChild : public Child{
public:
    int igrandchild;
    GrandChild():igrandchild(1000) {}
    virtual void f() { cout << "GrandChild::f()" << endl; }
    virtual void g_child() { cout << "GrandChild::g_child()" << endl; }
    virtual void h_grandchild() { cout << "GrandChild::h_grandchild()" << endl; }
};

//测试程序，用了一个int** pVtab 来作为遍历对象内存布局的指针，这样，就可以方便地像使用数组一样来遍历所有的成员包括其虚函数表了
typedef void(*Fun)(void);
GrandChild gc;
int** pVtab = (int**)&gc;

cout << "[0] GrandChild::_vptr->" << endl;
for (int i=0; (Fun)pVtab[0][i]!=NULL; i++){
     Fun pFun = (Fun)pVtab[0][i];
     cout << "    ["<<i<<"] ";
     pFun();
}
cout << "[1] Parent.iparent = " << (int)pVtab[1] << endl;
cout << "[2] Child.ichild = " << (int)pVtab[2] << endl;
cout << "[3] GrandChild.igrandchild = " << (int)pVtab[3] << endl;
```

运行结果如下：

```
[0] GrandChild::_vptr->
    [0] GrandChild::f()
    [1]  Parent::g()
    [2]  Parent::h()
    [3] GrandChild::g_child()
    [4] Child::h_child()
    [5] GrandChild::h_grandchild()
[1] Parent.iparent = 10
[2] Child.ichild = 100
[3] GrandChild.igrandchild = 1000
```

图片表示如下：

![img](https://p-blog.csdn.net/images/p_blog_csdn_net/haoel/EntryImages/20081015/dd02.jpg)

*(上面这张图有误，第4行应该是Child::h_child())*

可得到如下结果：

<font color=red>*虚函数表在最前面的位置*</font>

<font color=red>成员变量根据其继承和声明顺序依次放在后面</font>

<font color=red>在单一的继承中，被*overwrite*的虚函数在虚函数表中得到了更新</font>

**多重继承**

假设有下面的继承关系。注意Base1、Base2、Base3相互都是没有关系的类，子类覆盖了父类的f()，还有一个自己的g1()，而且每个类都有一个自己的成员变量：

![img](https://p-blog.csdn.net/images/p_blog_csdn_net/haoel/EntryImages/20081015/dd03.jpg)

源代码如下：

```c++
class Base1 {
public:
    int ibase1;
    Base1():ibase1(10) {}
    virtual void f() { cout << "Base1::f()" << endl; }
    virtual void g() { cout << "Base1::g()" << endl; }
    virtual void h() { cout << "Base1::h()" << endl; }
};

class Base2 {
public:
    int ibase2;
    Base2():ibase2(20) {}
    virtual void f() { cout << "Base2::f()" << endl; }
    virtual void g() { cout << "Base2::g()" << endl; }
    virtual void h() { cout << "Base2::h()" << endl; }
};

 
class Base3 {
public:
    int ibase3;
    Base3():ibase3(30) {}
    virtual void f() { cout << "Base3::f()" << endl; }
    virtual void g() { cout << "Base3::g()" << endl; }
    virtual void h() { cout << "Base3::h()" << endl; }
};

class Derive : public Base1, public Base2, public Base3 {
public:
    int iderive;
    Derive():iderive(100) {}
    virtual void f() { cout << "Derive::f()" << endl; }
    virtual void g1() { cout << "Derive::g1()" << endl; }
};

//测试程序，查看子类实例的内存布局，使用了一个s变量，其中用到了sizeof(Base)来找下一个类的偏移量
typedef void(*Fun)(void);
Derive d;
int** pVtab = (int**)&d;
cout << "[0] Base1::_vptr->" << endl;
pFun = (Fun)pVtab[0][0];
cout << "     [0] ";
pFun();
pFun = (Fun)pVtab[0][1];
cout << "     [1] ";pFun();
pFun = (Fun)pVtab[0][2];
cout << "     [2] ";pFun();
pFun = (Fun)pVtab[0][3];
cout << "     [3] "; pFun();
pFun = (Fun)pVtab[0][4];
cout << "     [4] "; cout<<pFun<<endl;
cout << "[1] Base1.ibase1 = " << (int)pVtab[1] << endl;

int s = sizeof(Base1)/4;
cout << "[" << s << "] Base2::_vptr->"<<endl;
pFun = (Fun)pVtab[s][0];
cout << "     [0] "; pFun();
Fun = (Fun)pVtab[s][1];
cout << "     [1] "; pFun();
pFun = (Fun)pVtab[s][2];
cout << "     [2] "; pFun();
pFun = (Fun)pVtab[s][3];
cout << "     [3] ";
cout<<pFun<<endl;
cout << "["<< s+1 <<"] Base2.ibase2 = " << (int)pVtab[s+1] << endl;

s = s + sizeof(Base2)/4;
cout << "[" << s << "] Base3::_vptr->"<<endl;
pFun = (Fun)pVtab[s][0];
cout << "     [0] "; pFun();
pFun = (Fun)pVtab[s][1];
cout << "     [1] "; pFun();
pFun = (Fun)pVtab[s][2];
cout << "     [2] "; pFun();
pFun = (Fun)pVtab[s][3];
cout << "     [3] ";
cout<<pFun<<endl;
s++;
cout << "["<< s <<"] Base3.ibase3 = " << (int)pVtab[s] << endl;
s++;
cout << "["<< s <<"] Derive.iderive = " << (int)pVtab[s] << endl;
```

运行结果如下：

```c++
[0] Base1::_vptr->

     [0] Derive::f()

     [1] Base1::g()

     [2] Base1::h()

     [3] Driver::g1()

     [4] 00000000      ç 注意：在GCC下，这里是1

[1] Base1.ibase1 = 10

[2] Base2::_vptr->

     [0] Derive::f()

     [1] Base2::g()

     [2] Base2::h()

     [3] 00000000      ç 注意：在GCC下，这里是1

[3] Base2.ibase2 = 20

[4] Base3::_vptr->

     [0] Derive::f()

     [1] Base3::g()

     [2] Base3::h()

     [3] 00000000

[5] Base3.ibase3 = 30

[6] Derive.iderive = 100

```

使用图片表示：

![img](https://p-blog.csdn.net/images/p_blog_csdn_net/haoel/EntryImages/20081015/dd04.jpg)

可得到如下结果：

<font color=red>每个父类都有自己的虚表，子类有多个虚函数指针，分别指向父类的虚表</font>

<font color=red>子类的成员函数被放到了第一个父类的表中，如g1()</font>

<font color=red>内存布局中，其父类布局依次按声明顺序排列</font>

<font color=red>每个父类的虚表中的f()函数都被*overwrite*成了子类的f()</font>

<font color=red>子类的成员变量在最后</font>

sizeof(Derive) 结果=28

**重复继承**

重复继承就是某个基类被间接的重复继承了多次，下图的继承关系重载了父类的f()

![img](https://p-blog.csdn.net/images/p_blog_csdn_net/haoel/EntryImages/20081015/dd05.jpg)

每个类都有两个变量，一个是整形（4字节），一个是字符（1字节），而且还有自己的虚函数，自己*overwrite*父类的虚函数。如子类*D*中，f()覆盖了超类的函数， f1() 和f2() 覆盖了其父类的虚函数，Df()为自己的虚函数。源代码如下：

```c++
class B
{
    public:
        int ib;
        char cb;
    public:
        B():ib(0),cb('B') {}
        virtual void f() { cout << "B::f()" << endl;}
        virtual void Bf() { cout << "B::Bf()" << endl;}
};

class B1 :  public B
{
    public:
        int ib1;
        char cb1;
    public:
        B1():ib1(11),cb1('1') {}
        virtual void f() { cout << "B1::f()" << endl;}
        virtual void f1() { cout << "B1::f1()" << endl;}
        virtual void Bf1() { cout << "B1::Bf1()" << endl;}
};

class B2:  public B
{
    public:
        int ib2;
        char cb2;
    public:
        B2():ib2(12),cb2('2') {}
        virtual void f() { cout << "B2::f()" << endl;}
        virtual void f2() { cout << "B2::f2()" << endl;}
        virtual void Bf2() { cout << "B2::Bf2()" << endl;}
};

class D : public B1, public B2
{
    public:
        int id;
        char cd;
    public:
        D():id(100),cd('D') {}
        virtual void f() { cout << "D::f()" << endl;}
        virtual void f1() { cout << "D::f1()" << endl;}
        virtual void f2() { cout << "D::f2()" << endl;}
        virtual void Df() { cout << "D::Df()" << endl;}
};

//测试代码查看子类内存布局
typedef void(*Fun)(void);

    int** pVtab = NULL;
    Fun pFun = NULL;
    D d;
    pVtab = (int**)&d;
    cout << "[0] D::B1::_vptr->" << endl;
    pFun = (Fun)pVtab[0][0];
    cout << "     [0] ";    pFun();
    pFun = (Fun)pVtab[0][1];
    cout << "     [1] ";    pFun();
    pFun = (Fun)pVtab[0][2];
    cout << "     [2] ";    pFun();
    pFun = (Fun)pVtab[0][3];
    cout << "     [3] ";    pFun();
    pFun = (Fun)pVtab[0][4];
    cout << "     [4] ";    pFun();
    pFun = (Fun)pVtab[0][5];
    cout << "     [5] 0x" << pFun << endl;

    cout << "[1] B::ib = " << (int)pVtab[1] << endl;
    cout << "[2] B::cb = " << (char)pVtab[2] << endl;
    cout << "[3] B1::ib1 = " << (int)pVtab[3] << endl;
    cout << "[4] B1::cb1 = " << (char)pVtab[4] << endl;
    
    cout << "[5] D::B2::_vptr->" << endl;
    pFun = (Fun)pVtab[5][0];
    cout << "     [0] ";    pFun();
    pFun = (Fun)pVtab[5][1];
    cout << "     [1] ";    pFun();
    pFun = (Fun)pVtab[5][2];
    cout << "     [2] ";    pFun();
    pFun = (Fun)pVtab[5][3];
    cout << "     [3] ";    pFun();
    pFun = (Fun)pVtab[5][4];
    cout << "     [4] 0x" << pFun << endl;
    
    cout << "[6] B::ib = " << (int)pVtab[6] << endl;
    cout << "[7] B::cb = " << (char)pVtab[7] << endl;   
    cout << "[8] B2::ib2 = " << (int)pVtab[8] << endl;
    cout << "[9] B2::cb2 = " << (char)pVtab[9] << endl;
    cout << "[10] D::id = " << (int)pVtab[10] << endl;
    cout << "[11] D::cd = " << (char)pVtab[11] << endl;
```

运行结果如下：

```
[0] D::B1::_vptr->
     [0] D::f()
     [1] B::Bf()
     [2] D::f1()
     [3] B1::Bf1()
     [4] D::Df()
     [5] 0x00000000
[1] B::ib = 0
[2] B::cb = B
[3] B1::ib1 = 11
[4] B1::cb1 = 1
[5] D::B2::_vptr->
     [0] D::f()
     [1] B::Bf()
     [2] D::f2()
     [3] B2::Bf2()
     [4] 0x00000000
[6] B::ib = 0
[7] B::cb = B
[8] B2::ib2 = 12
[9] B2::cb2 = 2
[10] D::id = 100
[11] D::cd = D
```

使用图片表示：

![img](https://p-blog.csdn.net/images/p_blog_csdn_net/haoel/EntryImages/20081015/dd06.jpg)

可得结论：

<font color=red>D有两份父类B的成员变量，一份是从B1继承来，一份从B2继承来</font>

<font color=red>D自己的成员函数放在第一个虚函数表中，如Df()，放在B1虚函数表</font>>

**多重虚拟继承**

虚拟继承就是为了解决重复继承中重复数据成员的问题，上述的重复继承，只需要把B1和B2继承B的语法中加上virtual，就变成了虚拟继承，如下：

![img](https://p-blog.csdn.net/images/p_blog_csdn_net/haoel/EntryImages/20081015/dd07.jpg)

```c++
class B {……};

class B1 : virtual public B{……};

class B2: virtual public B{……};

class D : public B1, public B2{ …… };
```

测试程序，查看B1的内存布局：

```c++
int** pVtab = NULL;
Fun pFun = NULL;

B1 bb1;
pVtab = (int**)&bb1;
cout << "[0] B1::_vptr->" << endl;
pFun = (Fun)pVtab[0][0];
cout << "     [0] ";
pFun(); //B1::f1();
cout << "     [1] ";
pFun = (Fun)pVtab[0][1];
pFun(); //B1::bf1();
cout << "     [2] ";
cout << pVtab[0][2] << endl;
cout << "[1] = 0x";
cout << (int)pVtab[1] << endl; //B1::ib1
cout << "[2] B1::ib1 = ";
cout << (int)pVtab[2] << endl; //B1::ib1
cout << "[3] B1::cb1 = ";
cout << (char)pVtab[3] << endl; //B1::cb1
cout << "[4] = 0x";
cout << (int*)*((int*)(&bb1) + 4) << endl; //NULL
cout << "[5] B::_vptr->" << endl;
pFun = (Fun)pVtab[5][0];
cout << "     [0] ";
pFun(); //B1::f();
pFun = (Fun)pVtab[5][1];
cout << "     [1] ";
pFun(); //B::Bf();
cout << "     [2] ";
cout << "0x" << (Fun)pVtab[5][2] << endl;
cout << "[6] B::ib = ";
cout << (int)*((int*)(&bb1) + 6) << endl; //B::ib
cout << "[7] B::cb = ";
```

运行结果如下：

```
[0] B1::_vptr->

     [0] B1::f1()

     [1] B1::Bf1()

     [2] 0

[1] = 0x00454310 ç该地址取值后是-4

[2] B1::ib1 = 11

[3] B1::cb1 = 1

[4] = 0x00000000

[5] B::_vptr->

     [0] B1::f()

     [1] B::Bf()

     [2] 0x00000000

[6] B::ib = 0

[7] B::cb = B

```

把超类放在了最后