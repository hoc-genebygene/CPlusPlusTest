#include <iostream>
#include <atomic>
#include <thread>

std::atomic<bool> done_flag { false };

int main() {
    int num = 1;

    auto thr_proc = [&num]() { num *= 3; done_flag = true; };

    std::thread thr(thr_proc);
    while (!done_flag) {};

    thr.join();

    std::cout << num << std::endl;

}
