功能
============
支持多进程多线程同时读写日志文件。轮转生成日志文件，定期检查磁盘容量并且自动清理旧日志文件。
agile_logger日志在内存池和并发锁方面有做优化，性能有不错的提升，支持多线程并发写入。
支持输出STL相关容器数据。
相关使用，可以参考test_case