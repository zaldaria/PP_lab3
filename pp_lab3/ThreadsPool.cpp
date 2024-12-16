#include "ThreadsPool.h"

threadsPool::threadsPool() {
    cntThreads = std::thread::hardware_concurrency();

    for (unsigned int i = 0; i < cntThreads; i++) {
#if defined (_WIN32) || defined (_WIN64)
        HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, [](void* param) -> unsigned {
            return static_cast<threadsPool*>(param)->run(param); }, this, 0, NULL);
        if (thread == nullptr) {
            DWORD err = GetLastError();
            printf("Cannot create thread, GetLastError=%u", err);
        }
        else {
            threads.emplace_back(thread);
        }
#elif defined __linux__
        pthread_t thread;
        int result = pthread_create(&thread, nullptr, [](void* param) -> void* {
            return static_cast<threadsPool*>(param)->run(param);  // Pass the param
            }, this);
        if (result == 0) {
            threads.emplace_back(thread);
        }
        else {
            printf("Cannot create thread, errno=%d", errno);
        }
#endif

    }
}

threadsPool::~threadsPool() {
    stop = true;
    condition.notify_all();
#if defined (_WIN32) || defined (_WIN64)
    WaitForMultipleObjects(GetCntThreads(), threads.data(), TRUE, INFINITE);
#endif

    for (unsigned int i = 0; i < cntThreads; i++) {
#if defined (_WIN32) || defined (_WIN64)
        CloseHandle(threads[i]);
#elif defined __linux__
        pthread_join(threads[i], nullptr);
#endif
    }
}

unsigned int threadsPool::GetCntThreads() {
    return cntThreads;
}

#if defined (_WIN32) || defined (_WIN64)
unsigned __stdcall threadsPool::run(void* param)
#elif defined __linux__
void* threadsPool::run(void* param)
#endif
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
#if defined (_WIN32) || defined (_WIN64)
    _endthreadex(0);
    return 0;
#elif defined __linux__
    return nullptr;
#endif
}

#if defined (_WIN32) || defined (_WIN64)
void threadsPool::passQ(function<void(double)> f, double p1)
#elif defined __linux__
void* threadsPool::passQ(function<void(double)> f, double p1)  
#endif
{
    {
        lock_guard<mutex> lg(m);
        task t = task(f, p1);
        q.push(t);
    }
    condition.notify_one();
#if defined __linux__
    return nullptr;
#endif
}
