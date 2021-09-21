#include "LoggerOutputToFile.h"

#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

namespace agile {
namespace logger {

bool LoggerOutputToFile::CheckFileName(size_t add_size, tm& cur_tm)
{
    if( cur_file_name_.empty() )
    {
        cur_tm_day_ = cur_tm;
        return GetCurLogName();
    }

    if (config_->logger_name_strict_with_date
        && cur_tm.tm_mday != cur_tm_day_.tm_mday)
    {
        cur_tm_day_ = cur_tm;
        return GetCurLogName();
    }

    if (cur_file_size_ + add_size > config_->logger_file_size_limit)
    {
        cur_tm_day_ = cur_tm;
        cur_file_size_ = 0;
        return GetCurLogName();  
    }
    return false;
}

bool LoggerOutputToFile::GetCurLogName()
{
    // TODO logging
    std::string temp_file_name;
    if (config_->logger_name_with_date)
    {
        char date_char[32];
        int ret_val = snprintf(date_char, sizeof(date_char), "%4d-%02d-%02d", 
                    cur_tm_day_.tm_year, cur_tm_day_.tm_mon, cur_tm_day_.tm_mday);
        temp_file_name = config_->logger_dir_path + "/" + config_->logger_name + "_" + std::string(date_char, ret_val);
    }
    else
    {
        temp_file_name = config_->logger_name;
    }

    if (cur_log_id_ > 1000)
    {
        cur_log_id_ = 0;
    }
    
    int count = 1000;
    while (count > 0)
    {
        --count;
        std::string name = temp_file_name + "_" + std::to_string(cur_log_id_++) + ".log"; 
        if (FILE *file = fopen(name.c_str(), "r"))
        {
            fclose(file);
        } 
        else
        {
            cur_file_name_ = name;
            return true;
        }
    }
    temp_file_name += ".log";
    
    if (temp_file_name == cur_file_name_)
    {
        return false;
    }
    cur_file_name_ = temp_file_name;
    return true;
}

void LoggerOutputToFile::OnLoggerData(const std::shared_ptr<LoggerData>& logger_data)
{
    size_t size = logger_data->logger_buffer->GetWriteIndex();

    CheckFileName(size, logger_data->tm_time);

    FILE* file_handler = fopen(cur_file_name_.c_str(), "a+");
    if (!file_handler)
    {
        std::cerr << "fail to open file:" << cur_file_name_ << std::endl;
        return;
    }
    fseek(file_handler, 0, SEEK_END);
    
    std::shared_ptr<std::string>& str = logger_data->logger_buffer->Data();

    fwrite(str->c_str(), size, 1, file_handler);

    fclose(file_handler);

    str->resize(size);
}

void LoggerOutputToFile::OnLoggerDatas(const std::list<std::shared_ptr<LoggerData>>& logger_data_list)
{
    FILE* file_handler = nullptr;
    for (auto& it : logger_data_list)
    {
        size_t size = it->logger_buffer->GetWriteIndex();
        if (CheckFileName(size, it->tm_time))
        {
            if (file_handler)
            {
                fclose(file_handler);
                file_handler = nullptr;
            }
            file_handler = fopen(cur_file_name_.c_str(), "a+");
        }
        if (file_handler)
        {
            fseek(file_handler, 0, SEEK_END);
            std::shared_ptr<std::string>& str = it->logger_buffer->Data();
            fwrite(str->c_str(), size, 1, file_handler);
            str->resize(size);
        }
        else
        {
           std::cerr << "fail to open file:" << cur_file_name_ << std::endl; 
        }
    }
    if (file_handler)
    {
        fclose(file_handler);
    }
}

} // namespace logger
} // namespace agile
