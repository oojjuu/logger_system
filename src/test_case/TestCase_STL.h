#pragma once

#include "TestDataObject.h"

/**
*@brief STL功能测试类
*/
struct TestCase_STL {
    static void Test();
};

void TestCase_STL::Test() {
    std::vector<int> vec_int = {1,2,3,4,5,6};  
    { LOG_INFO << "vec_int:" << vec_int; }

    std::list<std::string> list_str = {"aaa", "bbb"};
    { LOG_WARN << "list_str:" << list_str; }
    
    std::vector<TestDataObject*> vec_obj = {new TestDataObject(), new TestDataObject(), new TestDataObject() };
    { LOG_ERROR << "vec_obj:" << vec_obj; }
    
    for (auto& it : vec_obj) {
        delete it;
        it = nullptr;
    }

    std::map<std::string, std::vector<int> > map_vec = {{"aa11", {1,2,3}}, {"aa22", {111,222,333}}};
    { LOG_FATAL << "map_vec:" << map_vec; }

    std::map<std::string, std::vector<std::shared_ptr<TestDataObject>> > map_obj = {{"aa11", {std::make_shared<TestDataObject>(), std::make_shared<TestDataObject>()}},
                                                                                {"aa22", {std::make_shared<TestDataObject>(), std::make_shared<TestDataObject>()}} };
    { LOG_SYSTEM << "map_vec_obj:" << map_obj; }
}