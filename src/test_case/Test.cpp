
#include "Test.h"

#include "TestCase_Base.h"
#include "TestCase_STL.h"
#include "TestCase_Output.h"
#include "TestCase_Thread.h"

void Test::DoTest(const std::string& conf_path, int thread_num, uint32_t sleep_mcro_seconds, const std::string& tag) {
    agile::logger::InitLogger(conf_path, tag);

    agile::logger::SetLoggerOutput(1, std::make_shared<TestCase_Output>());
    
    TestCase_Base::Test();
    TestCase_STL::Test();
    
    TestCase_Thread thread_obj;
    thread_obj.Test(thread_num, sleep_mcro_seconds);
}