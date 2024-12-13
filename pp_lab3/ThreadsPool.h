#pragma once
#include <iostream>
#include <vector>
#include <future>
#include <thread>
#include <queue>
#include <functional>
#include <Windows.h>

using namespace std;

struct task {
    function<void(double)> func;
    double param1;

    task(function<void(double)> f, double p1): func(f), param1(p1) {}
};

class threadsPool {
private:
    //vector <thread> threads;
    vector<HANDLE> threads;

    uint32_t cntThreads = 0;
    queue <task> q;
    condition_variable condition;
    mutex m;
    bool stop = false;


public:

    threadsPool();
    ~threadsPool();
    unsigned int GetCntThreads();
    //void run();
    unsigned __stdcall run(void* param);
    unsigned __stdcall passQ(function<void(double)> f, double p1);

};
