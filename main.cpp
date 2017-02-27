#include <iostream>
#include <atomic>
#include <thread>

bool done_flag = false;

class Lock {
    std::atomic_flag lock_flag = ATOMIC_FLAG_INIT;

public:
    void lock() {
        // Spin until flag is cleared
        while (lock_flag.test_and_set()) {}
    }

    void unlock() {
        lock_flag.clear();
    }
};

int main() {
    int num = 1;

    Lock lock;

    auto thr_proc = [&num, &lock]() {
        lock.lock();
        num *= 3;
        done_flag = true;
        lock.unlock();
    };

    std::thread thr(thr_proc);

    while (true) {
        lock.lock();

        if (done_flag) {
            lock.unlock();
            break;
        }

        lock.unlock();
    }

    std::cout << num << std::endl;
    thr.join();
}
