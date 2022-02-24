#include <iostream>
#include <vector>

#include "Test.h"

using namespace std;

int main(int argc, char* argv[]) {
    std::cout << "args config_path thread_num sleep_micro_seconds tag" << std::endl;
    std::cout << "example: ./agile_logger_test ../conf/agile_logger.conf 0 0" << std::endl;
    if (argc < 4) {
        std::cerr << "argc num error " << std::endl;
        return 0;
    }
    std::string path = argv[1];
    int thread_num = atoi(argv[2]);
    int sleep_micro_seconds = atoi(argv[3]);

    std::string tag;
    if (argc > 4) {
        tag = argv[4];
    }

	std::cout << "go to run test case ..." << std::endl;
    Test test;
    test.DoTest(path, thread_num, sleep_micro_seconds, tag);
    std::cout << "finish run test case" << std::endl;
    return 0;
}