
#include <stdarg.h>
#include <iostream>
#include <thread>

#include "Logger.h"

using namespace agile::logger;

void TestThread(const std::string& s)
{
	LOG_TRACE << "["<< s << "]-----------------";
}

int main()
{
    agile::logger::InitLogger("../agile_logger_config.agile");
   // return 0;
	std::thread t1(TestThread,"aaaaa");
	t1.join();

    std::thread t2(TestThread,"bbbbb");
	t2.join();

    std::thread t3(TestThread,"ccccc");
	t3.join();

    std::cout << "test main " << std::endl;
    LOG_TRACE1("print format val:%d ,s:%s", 1010, "abc");
    std::string str("test logger");
  
    {
        int64_t val = 2372173217318937;

        long long val1 = 1132131237214921421;
       // std::cout << "test main 11" << std::endl;
       // LOG_TRACE << val;
        LOG_TRACE << str << " , char * test";
        LOG_DEBUG << str << " , char * test";
        LOG_INFO << str << " , char * test " << 123;
        LOG_WARN << str << " , char * test " << true;
        LOG_ERROR << str << " , char * test " << val;
        LOG_FATAL << str << " , char * test a " << 217318937;
        LOG_SYSTEM << str << " , char * test val1:" << val1;

        LOG_TAG_INFO(AgileLogger) << str << " , char * test";
    }
    LOG_TRACE1("print format val:%d ,s:%s", 1010, "abc");

    LOG_TAG_TRACE1(AgileLoggerFMT, "print format val:%d ,s:%s", 1010, "abc");

    LOG_CID_INFO(1) << str << " , char * test 111";

    //getchar();
    return 0;
}
