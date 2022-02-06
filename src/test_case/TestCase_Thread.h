#pragma once

#include <iostream>
#include <thread>
#include <chrono>
#include "Logger.h"

class TestCase_Thread
{
public:
	TestCase_Thread() = default;
	~TestCase_Thread() = default;

	void Test(int thread_num, uint32_t sleep_mcro_seconds);

private:
    void Run(uint32_t sleep_mcro_seconds);	

private:
    bool running_ = false;
    std::vector<std::shared_ptr<std::thread>> threads_;
};
