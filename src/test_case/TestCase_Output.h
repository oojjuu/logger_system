#pragma once

#include <iostream>

#include "Logger.h"
#include "LoggerOutput.h"

class TestCase_Output : public agile::logger::LoggerOutput {
public:
	TestCase_Output() = default;
	virtual ~TestCase_Output() = default;

    virtual void OnLoggerData(const std::shared_ptr<agile::logger::LoggerData>& logger_data) override {
        std::shared_ptr<std::string>& str = logger_data->logger_buffer->data();
        std::cout << "=========[TestCase_Output]:" <<  *str << std::endl;
    }
};
