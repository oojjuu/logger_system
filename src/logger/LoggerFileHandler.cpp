#include "LoggerFileHandler.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <sys/stat.h>

namespace agile {
namespace logger {

LoggerFileHandler::~LoggerFileHandler() {
     Close();
}

void LoggerFileHandler::Close() {
    if (file_handler_) {
        fclose(file_handler_);
        file_handler_ = nullptr;
    }
}

bool LoggerFileHandler::Open(const std::string& file_path) {
    if (file_handler_) {
        fclose(file_handler_);
    }
    file_handler_ = fopen(file_path.c_str(), "a+");
    if (!file_handler_) {
        std::cerr << "fail to open file:" << file_path << " err:" << strerror(errno) << std::endl;
        return false;
    }
    file_path_ = file_path;
    return true;
}

int64_t LoggerFileHandler::GetFileSize() const {
    if (!file_handler_) {
        return -1;
    }
    fseek(file_handler_, 0, SEEK_END);
    return ftell(file_handler_);
}

bool LoggerFileHandler::AsynWirte(const std::shared_ptr<LoggerData>& logger_data) {    
    if (!file_handler_) {
        return false;
    }

    size_t size = logger_data->logger_buffer->write_index();
    std::shared_ptr<std::string>& str = logger_data->logger_buffer->data();
    int res = fwrite(str->c_str(), size, 1, file_handler_);
    if (res != 1) {
        std::cerr << "fail to write data nmemb:1 but res:" << res  << " err:" << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool LoggerFileHandler::SyncWrite(const std::shared_ptr<LoggerData>& logger_data) {
    return AsynWirte(logger_data) ? Sync() : false;
}

bool LoggerFileHandler::Flush() {
    if (!file_handler_) {
        return false;
    }
    if (0 == fflush(file_handler_) ) {
        return true;
    }
    std::cerr << "fail to flush errno:" << (int)errno << " err:" << strerror(errno) << std::endl;
    return false;
}

bool LoggerFileHandler::Sync() {
    if (!file_handler_) {
        return false;
    }

    if (0 != fflush(file_handler_)) {
        std::cerr << "fail to flush " << file_path_ << " err:" << strerror(errno) << std::endl;
        return false;
    }
    if (0 != fsync(fileno(file_handler_))) {
        std::cerr << "fail to fsync " << file_path_ << " err:" << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

} // namespace logger
} // namespace agile