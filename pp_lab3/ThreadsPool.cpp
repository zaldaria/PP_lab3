#include "ThreadsPool.h"

threadsPool::threadsPool() {
    cntThreads = std::thread::hardware_concurrency();
    
    for (unsigned int i = 0; i < cntThreads; i++) {
        HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, [](void* param) -> unsigned {
            return static_cast<threadsPool*>(param)->run(param);
            }, this, 0, NULL);
        threads.emplace_back(thread);
    }
}

threadsPool::~threadsPool() {
    stop = true;
    condition.notify_all();

    WaitForMultipleObjects(GetCntThreads(), threads.data(), TRUE, INFINITE);

    for (unsigned int i = 0; i < cntThreads; i++) {
        CloseHandle(threads[i]);
    }
}

unsigned int __stdcall threadsPool::GetCntThreads()
{
    return cntThreads;
}

unsigned __stdcall threadsPool::run(void* param)
{
    while (true) {
        unique_lock<mutex> lk(m);
        condition.wait(lk, [this]() {return !q.empty() || stop; });
        if (!q.empty()) {
            task t = q.front();
            q.pop();
            lk.unlock();
            t.func(t.param1);
        }
        if (stop)
            break;

    }
    _endthreadex(0);
    return 0;
}

unsigned __stdcall threadsPool::passQ(function<void(double)> f, double p1)
{
    {
        lock_guard<mutex> lg(m);
        task t = task(f, p1);
        q.push(t);
    }

    condition.notify_one();
    return 0;
}

