#include "LoggerOutputToFile.h"

#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#ifdef __linux__
#include <sys/statfs.h>
#else
#include <sys/param.h>
#include <sys/mount.h>
#endif // __linux__

namespace agile {
namespace logger {

LoggerOutputToFile::~LoggerOutputToFile()
{
    if (file_handler_list_)
    {
        FileHandlerNode* node = file_handler_list_->Tail();
        while (node)
        {
            if (!node->is_finish)
            {
                 Rename(node->logger_id, node->file_path);
            }
            node = node->prev;
        }
    }
}

void* LoggerOutputToFile::PreviousCheck(std::shared_ptr<LoggerData>& logger_data)
{
    uint32_t file_date_time = logger_data->tm_time.tm_year;
    file_date_time <<= 4;
    file_date_time |= logger_data->tm_time.tm_mon;
    file_date_time <<= 6;
    file_date_time |= logger_data->tm_time.tm_mday;

    if (!file_handler_list_)
    {
        if (!CreateLoggerFile(file_date_time, 1))
        {
            return nullptr;
        }
    }

    FileHandlerNode* tail = file_handler_list_->Tail();
    if (tail->file_size >= config_->logger_file_size)
    {
        if (!CreateLoggerFile(file_date_time, 2))
        {
            return nullptr;
        }
    }
    
    if (config_->logger_split_with_date && tail->file_date_time != file_date_time)
    {
        if (!CreateLoggerFile(file_date_time, 3))
        {
            return nullptr;
        }
    }
    return file_handler_list_->Tail();
}

bool LoggerOutputToFile::CreateLoggerFile(uint32_t file_date_time, int create_type)
{
    std::lock_guard<std::mutex> lock(mtx_);
    switch (create_type)
    {
        case 1:
            {
                if (file_handler_list_)
                {
                    return true;
                }
            }
            break;
        case 2:
            {
                FileHandlerNode* tail = file_handler_list_->Tail();
                if (tail && tail->file_size < config_->logger_file_size)
                {
                    return true;
                }
            }
            break;
        case 3:
            {
                FileHandlerNode* tail = file_handler_list_->Tail();
                if (tail && tail->file_date_time == file_date_time)
                {
                    return true;
                }
            }
            break;
        default:
            break;
    }
    std::string file_path = GetLoggerFileName();
    if (file_path.empty())
    {
        return false;
    }

    std::shared_ptr<LoggerFileHandler> file_handler = std::make_shared<LoggerFileHandler>();
    if (!file_handler->Open(file_path))
    {
        std::cerr << "fail to open file:" << file_path << std::endl;
        return false;
    }
    file_name_map_[cur_log_id_] = file_path;

    FileHandlerNode* node = new FileHandlerNode(file_handler);
    node->logger_id = cur_log_id_;
    node->file_date_time = file_date_time;
    node->file_path = file_path;

    if (!file_handler_list_)
    {
        std::shared_ptr<FileHandlerList> file_handler_list = std::make_shared<FileHandlerList>();
        file_handler_list->EmplaceBack(node);
        file_handler_list_ = file_handler_list;
        CheckAndDeleteFileNameMap();
        return true;
    }

    FileHandlerNode* tail = file_handler_list_->Tail();
    if (tail)
    {
        tail->file_path = Rename(tail->logger_id, tail->file_path);
        tail->is_finish = true;
        tail->finish_time = std::chrono::steady_clock::now();
        tail->file_handler->Flush();
        file_name_map_[tail->logger_id] = tail->file_path;
    }
    file_handler_list_->EmplaceBack(node);

    if (file_handler_list_->Size() > config_->logger_file_number)
    {
        auto del_node = file_handler_list_->PopFront();
        if (del_node)
        {
            del_node->file_handler->Sync();
            del_node->file_handler->Close();
            auto it = file_name_map_.find(del_node->logger_id);
            if (it != file_name_map_.end())
            {
                remove(it->second.c_str());
                file_name_map_.erase(it);
            }
        }
    }
    CheckAndDeleteFileNameMap();
    return true;
}

void LoggerOutputToFile::CheckAndDeleteFileNameMap()
{
    int64_t size = (int64_t)file_name_map_.size();
    while (size-- > 0 && file_name_map_.size() > config_->logger_file_number)
    {
        auto it = file_name_map_.begin();
        auto node = file_handler_list_->IsExistLoggerId(it->first);
        if (node)
        {
            node->file_handler->Close();
        }
        remove(it->second.c_str());
        file_name_map_.erase(it);
    }
}

void LoggerOutputToFile::OnLoggerData(const std::shared_ptr<LoggerData>& logger_data)
{
    if (!logger_data->ptr)
    {
        return;
    }
    
    FileHandlerNode* node = (FileHandlerNode*)logger_data->ptr;
    node->file_size += logger_data->logger_buffer->GetWriteIndex();
    if (config_->save_real_time)
    {
        node->file_handler->SyncWrite(logger_data);
        return;
    }

    if (config_->save_on_logger_num == 0)
    {
        node->file_handler->AsynWirte(logger_data);
        return;
    }
    ++write_counter_;
    if (write_counter_ > config_->save_on_logger_num)
    {
        write_counter_ = 0;
        node->file_handler->SyncWrite(logger_data);
    }
}

void LoggerOutputToFile::Sync()
{
    if (!file_handler_list_)
    {
        return;
    }
    FileHandlerNode* node = file_handler_list_->Tail();
    while (node && !node->is_finish)
    {
        node->file_handler->Sync();
        node = node->prev;
    }
}

void LoggerOutputToFile::Flush()
{
    if (!file_handler_list_)
    {
        return;
    }
    FileHandlerNode* node = file_handler_list_->Tail();
    while (node && !node->is_finish)
    {
        node->file_handler->Flush();
        node = node->prev;
    }
}

void LoggerOutputToFile::CheckFiles()
{
    if (!file_handler_list_)
    {
        return;
    }

    std::chrono::steady_clock::time_point cur_time = std::chrono::steady_clock::now();
    FileHandlerNode* node = file_handler_list_->Tail();
    while (node)
    {
        if (node->is_finish)
        {
            if (node->file_handler->IsClosed())
            {
                break;
            }
            if (cur_time > node->finish_time + std::chrono::seconds(5))
            {
                std::lock_guard<std::mutex> lock(mtx_);
                node->file_handler->Sync();
                node->file_handler->Close();
            }
        }
        else
        {
            int64_t file_size = GetFileSize(node->file_path);
            if (file_size > 0)
            {
                node->file_size = (uint64_t)file_size;
            }
        }
        node = node->prev;
    }
}

void LoggerOutputToFile::CheckDisk()
{
    long long free_size = GetDiskFreeCapacity(config_->logger_dir_path);
    if (disk_free_size_limit_ == 0)
    {
        disk_free_size_limit_ = (long long)config_->logger_buffer_size_limit * 10;
    }
    if (free_size > disk_free_size_limit_)
    {
        return;
    }

    int64_t tail_id = -1;
    FileHandlerNode* node = file_handler_list_->Tail();
    if (node)
    {
        tail_id = node->logger_id;
    }
    std::vector<std::string> del_files;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        size_t map_size = file_name_map_.size();
        int num = (int)map_size/2;
        auto it = file_name_map_.begin();
        while (map_size > 2 && it != file_name_map_.end() && num-- > 0)
        {
            if (tail_id == (int64_t)it->first)
            {
                ++it;
                continue;
            }
            auto res_node = file_handler_list_->IsExistLoggerId(it->first);
            if (res_node)
            {
                res_node->file_handler->Sync();
                res_node->file_handler->Close();
            }
            del_files.emplace_back(it->second);
            it = file_name_map_.erase(it);
        }
    }
    for (auto& it : del_files)
    {
        remove(it.c_str());
    }
}

long long LoggerOutputToFile::GetDiskFreeCapacity(const std::string& path)
{
    struct statfs buffer;
    int res = statfs(path.c_str(), &buffer);
    return res < 0 ? res : (long long)buffer.f_bsize * (long long)buffer.f_bfree;
}

int64_t LoggerOutputToFile::GetFileSize(const std::string& file_name)
{
    struct stat statbuf;
    return 0 == stat(file_name.c_str(), &statbuf) ? statbuf.st_size : -1;
}

std::string LoggerOutputToFile::Rename(uint32_t logger_id, const std::string& loging_file)
{
    static const std::string kLogingString = "loging";
    if (loging_file.length() <= kLogingString.length())
    {
        return "";
    }
    size_t index = loging_file.length() - kLogingString.length();
    for (size_t i = 0; i < kLogingString.length(); i++)
    {
        if (loging_file[i + index] != kLogingString[i])
        {
            return "";
        }
    }
    if (0 != access(loging_file.c_str(), 0))
    {
        return "";
    }
    std::string new_name = loging_file.substr(0, loging_file.length() - 3);
    rename(loging_file.c_str(), new_name.c_str());
    return new_name;
}

std::string LoggerOutputToFile::GetLoggerFileName()
{
    if (file_handler_list_)
    {
        return GetLoggerFileNameByLoggerId(cur_log_id_ + 1, cur_log_id_);
    }

    std::vector<std::string> files_name;
    std::string fliter = config_->logger_name + "_" + file_name_tag_;
    if (!GetLoggerFiles(config_->logger_dir_path, fliter, files_name))
    {
        return "";
    }

    std::vector<int64_t> logger_ids;
    uint32_t res_id = GetLoggerOrderId(files_name, logger_ids);

    file_name_map_.clear();
    for (size_t i = 0; i < files_name.size(); i++)
    {
        if (logger_ids[i] < 0)
        {
            continue;
        }
        std::string& str = files_name[i];
        if (str.rfind(".loging") != std::string::npos && str.substr(str.length() - 6, 6) == "loging")
        {
            std::string log_name = str.substr(0, str.length() - 3);
            rename(str.c_str(), log_name.c_str());
            files_name[i] = log_name;
        }
        file_name_map_[logger_ids[i]] = files_name[i];
    }
    return GetLoggerFileNameByLoggerId(res_id, cur_log_id_);
}

std::string LoggerOutputToFile::GetLoggerFileNameByLoggerId(uint32_t logger_id, uint32_t& new_logger_id)
{
    std::string file_path = config_->logger_dir_path;
    if (file_path.back() != '/')
    {
        file_path += "/";
    }
    file_path += config_->logger_name;
    if (!file_name_tag_.empty())
    {
        file_path += ("_" + file_name_tag_);
    }
    file_path += "_";

    uint32_t temp_id = logger_id;
    std::string file_name = file_path + std::to_string(logger_id) + ".loging";
    std::string file_name_log = file_path + std::to_string(logger_id) + ".log";
    int loop_num = (int)config_->logger_file_number + 1;
    int ret1 = access(file_name.c_str(), 0);
    int ret2 = access(file_name_log.c_str(), 0);
    while ( (0 == ret1 || 0 == ret2 ) && --loop_num > 0)
    {
        if (0 == ret1)
        {
            int64_t file_size = GetFileSize(file_name);
            if (file_size > 0 && file_size < (int64_t)config_->logger_file_size)
            {
                break;
            }
        }
        file_name = file_path + std::to_string(++logger_id) + ".loging";
        file_name_log = file_path + std::to_string(logger_id) + ".log";
    }
    if (loop_num < 1)
    {
        logger_id = temp_id;
        file_name = file_path + std::to_string(logger_id) + ".loging";
        remove(file_name.c_str());
    }
    new_logger_id = logger_id;
    return file_name;
}

uint32_t LoggerOutputToFile::GetLoggerOrderId(const std::vector<std::string>& files_name, 
                                                                std::vector<int64_t>& logger_ids)
{
    if (files_name.empty())
    {
        return 0;
    }
    size_t size = files_name.size();
    logger_ids.resize(size);

    uint32_t max_id = 0;
    for (size_t i = 0; i < size; i++)
    {
        const std::string& str = files_name[i];
        size_t pos1 = str.rfind('.');
        size_t pos2 = str.rfind('_');
        if (pos1 == std::string::npos || pos2 == std::string::npos)
        {
            logger_ids[i] = -1;
            continue;
        }
        std::string num_str = str.substr(pos2 + 1, pos1 - pos2 - 1);
        uint32_t id_val = (uint32_t)atol(num_str.c_str());
        logger_ids[i] = (int64_t)id_val;
        if (id_val >= max_id)
        {
            max_id = id_val + 1;
        }
    }
    return max_id;
}

bool LoggerOutputToFile::GetLoggerFiles(const std::string& path, const std::string& fliter, std::vector<std::string>& res)
{
    DIR* direct_ptr = opendir(path.c_str());
    if (!direct_ptr)
    {
        std::cerr << "fail to opendir dir_path:" << path << std::endl;
        return false;
    }
    std::string pre_path = path;
    if (pre_path.back() != '/')
    {
        pre_path += "/";
    }
    struct dirent* entry = nullptr;
    while ((entry = readdir(direct_ptr)))
    {
        if (entry->d_name[0] == '.')
        {
            continue;
        }

        std::string name = entry->d_name;
        std::string file_path = pre_path + name;
        struct stat buffer;
        if(!(stat(file_path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode)))
        {
            continue;
        }
        if (name.find(fliter) != std::string::npos)
        {
            res.emplace_back(std::move(file_path));
        }
    }
    return true;
}

bool LoggerOutputToFile::CheckAndCreateLogDir(const std::string& dir_path)
{
    if (dir_path.empty())
    {
        std::cerr << "dir_path is empty" << std::endl;
        return false;
    }

    DIR* dir = opendir(dir_path.c_str());
    if (dir)
    {
        closedir(dir);
        return true;
    }
    
    std::string path = dir_path;
    if (path.back() != '/')
    {
        path += '/';
    }
    size_t pre_index = 0;
    size_t pos = 0;
    while ((pos = path.find_first_of('/', pre_index)) != std::string::npos)
    {
        std::string dir_str = path.substr(0, pos++);
        pre_index = pos;
        if (dir_str.empty())
        {
            continue;
        }
        if (0 == mkdir(dir_str.c_str(), 0777))
        {
            continue;
        }
        if (errno != EEXIST)
        {
            std::cerr << "fail to mkdir dir_path:" << dir_str << std::endl;
            return false;
        }
    }
    return true;
}

} // namespace logger
} // namespace agile