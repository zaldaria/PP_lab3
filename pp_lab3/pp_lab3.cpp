// HANDLE
// CreateMutex CreateThead  CloseHandle 
// Handle WINAPI CreateSemaphore
// InitializeCriticalSection LeaveCriticalSection DeleteCriticalSection EnterCriticalSection
// CreateThead --> _beginthread / _beginthraedex !
// unsigned Counter
// unsigned __stdcall SecondThread(void* pParam) {
//	while (Counter < 100000)
//		Counter++;
//	_endthread(0);
//	return(0);
//}
//hThread = (HANDLE)_beginthreadex(NULL, 0, &Secondthread, NULL, 0, &ThreadId);
//WaitForSingleObject(hThread, INFINITE);
//CloseHandle(hThread);

#include "ThreadsPool.h"
#include <iostream>
#include <random>

using namespace std;

const double PI = 3.14159265358979323846;

double factorial(int n) {
    if (n <= 1) return 1;
    double result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

vector<double> generateRandomArray(int n) {
    vector<double> array(n);

    random_device rd;  
    mt19937 gen(rd());
    uniform_real_distribution<> dis(1e-300, 1e-10);

    for (int i = 0; i < n; ++i) {
        array[i] = dis(gen);
    }

    return array;
}

void my_cos(double x) {
    x = fmod(x, 2 * PI);

    double  avgSum = 0.0;
    int N = 1000000;
    vector<double> tolerance = generateRandomArray(N);
    double term;
    double sum;
    int n;

    for (int i = 0; i < N; i++) {
        sum = 0.0;
        n = 0;
        do {
            term = (n % 2 == 0 ? 1 : -1) * (pow(x, 2 * n) / factorial(2 * n));
            sum += term;
            n++;
        } while (fabs(term) >= tolerance[i]); 
        avgSum += sum;
    }
    avgSum /= N;
    printf("Cosinus of %.2f = %.30f\n", x, avgSum);
}


int main()
{
    threadsPool pool;
    uint32_t n = pool.GetCntThreads();
    printf("Number of Threads: %i\n", n);

    if (n != 0) {
        for (uint32_t i = 0; i < n; i++) {
            pool.passQ(my_cos, PI * i / 2);
        }
    }
    else {
        printf("No free threads..");
    }

}