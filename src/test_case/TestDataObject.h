#pragma once

#include "Logger.h"

struct TestDataObject {
    TestDataObject() = default;
    TestDataObject(int val, const std::string& key):val(val),key(key){}
    ~TestDataObject() = default;

    void OnAgileLogger(agile::logger::LoggerBuffer& log) const {
        log << "{logger data obj key:" << key << " val:" << val << "}";
    }
    int val = 123;
    std::string key = "abcdef";
};