#pragma once

#include "TestDataObject.h"

using namespace agile::logger;

/**
*@brief 基础功能测试类
*/
struct TestCase_Base {
    static void Test();
};

void TestCase_Base::Test() {
    bool bool_val = true;
    char char_val = 123;
    uint8_t uint8_t_val = 12;
    int int_val = -100;
    uint32_t uint_val = 200;
    int64_t int64_t_val = 10023920;
    float f_val = 123.456;
    double d_val = 567.89;
    std::string str = "abcdefgh";

    TestDataObject obj1(123321, "hello");
    TestDataObject* obj2 = &obj1;
    std::shared_ptr<TestDataObject> obj3 = std::make_shared<TestDataObject>(987, "hihihi");

    LOG_TRACE << "bool:" <<  bool_val << ", char:" <<  char_val << ", uint8_t:" <<  uint8_t_val
              << ", int:" <<  int_val << ", uint:" <<  uint_val << ", int64_t:" <<  int64_t_val
              << ", float:" <<  f_val << ", double:" <<  d_val << ", str:" <<  str << ", obj1:" << obj1
              << ", obj2:" << obj2 << ", obj3:" << obj3;
}