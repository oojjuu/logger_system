
#include "TestCase_Thread.h"
#include <sstream>
#include <atomic>
#include <functional>

using namespace agile::logger;

std::atomic<int> write_file_line_num;

void TestCase_Thread::Test(int thread_num, uint32_t sleep_mcro_seconds) {
    running_ = true;
    write_file_line_num = 0;
    for (int i = 0; i < thread_num; i++) {
        running_ = true;
        threads_.emplace_back(std::make_shared<std::thread>(std::bind(&TestCase_Thread::Run, this, sleep_mcro_seconds)));
    }
    while (running_ && thread_num > 0) {
        std::cout << "time:" << time(0) << " write_file_line_num:" << write_file_line_num << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    for (auto &it : threads_) {
        if (it->joinable()) {
            it->join();
        }
    }
}

void TestCase_Thread::Run(uint32_t sleep_mcro_seconds) {
    while (running_) {
        LOG_DEBUG;
        ++write_file_line_num;
        if (sleep_mcro_seconds > 0) {
            std::this_thread::sleep_for(std::chrono::microseconds(sleep_mcro_seconds));
        }
    }
}