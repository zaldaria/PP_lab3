#pragma once
#include <iostream>
#include <vector>
#include <future>
#include <thread>
#include <queue>
#include <functional>
#if defined (_WIN32) || defined (_WIN64)
#include <Windows.h>
#elif defined __linux__
#include <pthread.h>
#include <errno.h>
#endif

using namespace std;

struct task {
    function<void(double)> func;
    double param1;

    task(function<void(double)> f, double p1): func(f), param1(p1) {}
};

class threadsPool {
private:
#if defined (_WIN32) || defined (_WIN64)
    vector<HANDLE> threads;
#elif defined __linux__
    vector<pthread_t> threads;
#endif

    uint32_t cntThreads = 0;
    queue <task> q;
    condition_variable condition;
    mutex m;
    bool stop = false;


public:

    threadsPool();
    ~threadsPool();
    unsigned int GetCntThreads();

#if defined (_WIN32) || defined (_WIN64)
    unsigned __stdcall run(void* param);
    void passQ(function<void(double)> f, double p1);
#elif defined __linux__
    void* run(void* param);
    void* passQ(function<void(double)> f, double p1);
#endif
   

};
