#pragma once

#include <string>
#include "Logger.h"

class Test
{
public:
	Test() = default;
	~Test() = default;

	void DoTest(const std::string& conf_path, int thread_num, uint32_t sleep_mcro_seconds, const std::string& tag);	
};