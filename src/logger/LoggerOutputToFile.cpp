#include "LoggerOutputToFile.h"

#include "LoggerUtil.h"

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

LoggerOutputToFile::~LoggerOutputToFile() {
    if (file_handler_list_) {
        FileHandlerNode* node = file_handler_list_->tail();
        while (node) {
            if (!node->is_finish) {
                LoggerUtil::RenameLogingToLog(node->file_path);
            }
            node = node->prev;
        }
    }
}

void LoggerOutputToFile::PreviousCheck(const std::shared_ptr<LoggerData>& logger_data) {
    uint32_t file_date_time = logger_data->tm_time.tm_year;
    file_date_time <<= 4;
    file_date_time |= logger_data->tm_time.tm_mon;
    file_date_time <<= 6;
    file_date_time |= logger_data->tm_time.tm_mday;
    logger_data->ptr = nullptr;
    int type_val = GetCreateType(file_date_time);
    if (type_val > 0) {
        if (!CreateLoggerFile(file_date_time, type_val)) {
            return;
        }
    }
    logger_data->ptr = file_handler_list_->tail();
}

int LoggerOutputToFile::GetCreateType(uint32_t file_date_time) {
    if (!file_handler_list_) {
        return 1;
    }

    FileHandlerNode* tail = file_handler_list_->tail();
    if (tail->file_size >= config_->logger_file_size) {
        return 2;
    }

    if (config_->logger_split_with_date && tail->file_date_time != file_date_time) {
        return 3;
    }
    return -1;
}

bool LoggerOutputToFile::IsCreated(uint32_t file_date_time, int create_type) {
    switch (create_type) {
        case 1: {
                if (file_handler_list_) {
                    return true;
                }
            }
            break;
        case 2: {
                FileHandlerNode* tail = file_handler_list_->tail();
                if (tail && tail->file_size < config_->logger_file_size) {
                    return true;
                }
            }
            break;
        case 3: {
                FileHandlerNode* tail = file_handler_list_->tail();
                if (tail && tail->file_date_time == file_date_time) {
                    return true;
                }
            }
            break;
        default:
            break;
    }
    return false;
}

bool LoggerOutputToFile::CreateLoggerFile(uint32_t file_date_time, int create_type) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (IsCreated(file_date_time, create_type)) {
        return true;
    }
    std::string file_path = GetLoggerFileName();
    if (file_path.empty()) {
        return false;
    }

    std::shared_ptr<LoggerFileHandler> file_handler = std::make_shared<LoggerFileHandler>();
    if (!file_handler->Open(file_path)) {
        std::cerr << "fail to open file:" << file_path << std::endl;
        return false;
    }
    file_name_map_[cur_log_id_] = file_path;

    static uint32_t kMaxLoggerIdLimit = 1000;//100000000;
    if (kMaxLoggerIdLimit < cur_log_id_) {
        RenameAllFilesLoggerId();
    }
    
    FileHandlerNode* node = new FileHandlerNode(cur_log_id_, file_date_time, file_path, file_handler);
    if (!file_handler_list_) {
        file_handler_list_ = std::make_shared<FileHandlerList>();
        file_handler_list_->EmplaceBack(node);
        CheckAndDeleteFileNameMap();
        return true;
    }

    FileHandlerNode* tail = file_handler_list_->tail();
    if (tail) {
        tail->file_path = LoggerUtil::RenameLogingToLog(tail->file_path);
        tail->is_finish = true;
        tail->finish_time = std::chrono::steady_clock::now();
        tail->file_handler->Flush();
        file_name_map_[tail->logger_id] = tail->file_path;
    }
    file_handler_list_->EmplaceBack(node);

    if (file_handler_list_->size() > config_->logger_file_number) {
        auto del_node = file_handler_list_->PopFront();
        if (del_node) {
            del_node->file_handler->Close();
            remove(del_node->file_path.c_str());
            file_name_map_.erase(del_node->logger_id);
        }
    }
    CheckAndDeleteFileNameMap();
    return true;
}

void LoggerOutputToFile::RenameAllFilesLoggerId() {
    std::map<uint32_t, std::string> new_file_name_map;
    uint32_t start_id = 0;
    for (auto& it : file_name_map_) {
        size_t pos1 = it.second.rfind('.');
        size_t pos2 = it.second.rfind('_');
        if (pos1 == std::string::npos || pos2 == std::string::npos) {
            continue;
        }

        std::string file_name = it.second.substr(0, pos2) + "_" + std::to_string(start_id) + it.second.substr(pos1);
        new_file_name_map[start_id] = file_name;
        rename(it.second.c_str(), file_name.c_str());

        if (file_handler_list_) {
            auto node = file_handler_list_->IsExistLoggerId(it.first);
            if (node) {
                node->file_path = file_name;
                node->logger_id = start_id;
            }
        }
        cur_log_id_ = start_id++;
    } 
    file_name_map_.swap(new_file_name_map);
}

void LoggerOutputToFile::CheckAndDeleteFileNameMap() {
    int64_t size = (int64_t)file_name_map_.size();
    while (size-- > 0 && file_name_map_.size() > config_->logger_file_number) {
        auto it = file_name_map_.begin();
        auto node = file_handler_list_->IsExistLoggerId(it->first);
        if (node) {
            node->file_handler->Close();
        }
        remove(it->second.c_str());
        file_name_map_.erase(it);
    }
}

void LoggerOutputToFile::Run(const std::chrono::steady_clock::time_point& cur_time) {
    if (!file_handler_list_) {
        return;
    }

    bool sys_call = sysc_time_val_ > cur_time;
    if (sys_call) {
        sysc_time_val_ = cur_time + std::chrono::milliseconds(config_->save_interval_msec);
    }

    FileHandlerNode* node = file_handler_list_->tail();
    while (node && !node->is_finish) {
        node->file_handler->Flush();
        if (sys_call) {
            node->file_handler->Sync();
        }
        node = node->prev;
    }

    static uint32_t kCheckFilesInterval = 10000;
    if (check_file_time_val_ < cur_time) {
        check_file_time_val_ = cur_time + std::chrono::milliseconds(kCheckFilesInterval);
        CheckFiles(cur_time);
    }

    static uint32_t kCheckDiskInterval = 300000;
    if (check_disk_time_val_ < cur_time) {
        check_disk_time_val_ = cur_time + std::chrono::milliseconds(kCheckDiskInterval);
        CheckDisk();
    }
}

void LoggerOutputToFile::OnLoggerData(const std::shared_ptr<LoggerData>& logger_data) {
    if (!logger_data->ptr) {
        return;
    }
    
    FileHandlerNode* node = (FileHandlerNode*)logger_data->ptr;
    node->file_size += logger_data->logger_buffer->write_index();
    if (config_->save_real_time) {
        node->file_handler->SyncWrite(logger_data);
        return;
    }
    node->file_handler->AsynWirte(logger_data);
    ++write_counter_;
    
    if (config_->save_on_logger_num > 0 && write_counter_ > config_->save_on_logger_num) {
        write_counter_ = 0;
        node->file_handler->Sync();
    }
}

void LoggerOutputToFile::CheckFiles(const std::chrono::steady_clock::time_point& cur_time) {
    FileHandlerNode* node = file_handler_list_->tail();
    while (node) {
        if (node->is_finish) {
            if (node->file_handler->IsClosed()) {
                break;
            }
            if (cur_time > node->finish_time + std::chrono::seconds(5)) {
                std::lock_guard<std::mutex> lock(mtx_);
                node->file_handler->Sync();
                node->file_handler->Close();
            }
        }
        else {
            int64_t file_size = LoggerUtil::GetFileSize(node->file_path);
            if (file_size > 0) {
                node->file_size = (uint64_t)file_size;
            }
        }
        node = node->prev;
    }
}

void LoggerOutputToFile::CheckDisk() {
    long long free_size = LoggerUtil::GetDiskFreeCapacity(config_->logger_dir_path);
    if (disk_free_size_limit_ == 0) {
        disk_free_size_limit_ = (long long)config_->logger_buffer_size_limit * 10;
    }
    if (free_size > disk_free_size_limit_) {
        return;
    }

    FileHandlerNode* node = file_handler_list_->tail();
    int64_t tail_id = node ? node->logger_id : -1;
 
    std::vector<std::string> del_files;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        size_t map_size = file_name_map_.size();
        int num = (int)map_size/2;
        auto it = file_name_map_.begin();
        while (map_size > 2 && it != file_name_map_.end() && num-- > 0) {
            if (tail_id == (int64_t)it->first) {
                ++it;
                continue;
            }
            auto res_node = file_handler_list_->IsExistLoggerId(it->first);
            if (res_node) {
                res_node->file_handler->Sync();
                res_node->file_handler->Close();
            }
            del_files.emplace_back(it->second);
            it = file_name_map_.erase(it);
        }
    }
    for (auto& it : del_files) {
        remove(it.c_str());
    }
}

std::string LoggerOutputToFile::GetLoggerFileName() {
    if (file_handler_list_) {
        return LoggerUtil::GetLoggerFileNameByLoggerId(config_, file_name_tag_, cur_log_id_ + 1, cur_log_id_);
    }

    std::vector<std::string> files_name;
    std::string fliter = config_->logger_name + "_" + file_name_tag_;
    if (!LoggerUtil::GetLoggerFiles(config_->logger_dir_path, fliter, files_name)) {
        return "";
    }

    std::vector<int64_t> logger_ids;
    uint32_t res_id = LoggerUtil::GetLoggerOrderIds(files_name, logger_ids);

    file_name_map_.clear();
    for (size_t i = 0; i < files_name.size(); i++) {
        if (logger_ids[i] < 0) {
            continue;
        }
        std::string& str = files_name[i];
        if (str.rfind(".loging") != std::string::npos && str.substr(str.length() - 6, 6) == "loging") {
            std::string log_name = str.substr(0, str.length() - 3);
            rename(str.c_str(), log_name.c_str());
            files_name[i] = log_name;
        }
        file_name_map_[logger_ids[i]] = files_name[i];
    }
    return LoggerUtil::GetLoggerFileNameByLoggerId(config_, file_name_tag_, res_id, cur_log_id_);
}

} // namespace logger
} // namespace agile