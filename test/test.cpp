#include<iostream>
#include<vector>
#include "../Server/MainServer/MainClient.h"
using namespace std;

 
int fun1(MainClient&);
int fun2(MainClient&);
int fun3(MainClient&);
int fun4(MainClient&);
int main()
{
    vector<int(*)(int,int)> v;//vector<decltype(fun1)*> v;
    v.push_back(fun1);
    v.push_back(fun2);
    v.push_back(fun3);
    v.push_back(fun4);
    cout<<v[0](1,2)<<endl;
    cout<<v[1](1,2)<<endl;
    cout<<v[2](1,2)<<endl;
    cout<<v[3](1,2)<<endl;
    return 0;
}
int fun1(int x,int y)
{
    return (x+y);
}
int fun2(int x,int y)
{
    return (x-y);
}
int fun3(int x,int y)
{
    return (x*y);
}
int fun4(int x,int y)
{
    return (x/y);
}