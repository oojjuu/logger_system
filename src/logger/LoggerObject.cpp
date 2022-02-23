#include "LoggerObject.h"

#include <assert.h>
#include "LoggerConfigManager.h"
#include "LoggerOutputToConsole.h"
#include "LoggerOutputToFile.h"

namespace agile {
namespace logger {

LoggerObject::LoggerObject(int conf_id, const std::string& file_name_tag) {
    config_ = LoggerConfigManager::GetInstance().GetConfig(conf_id);
    sysc_time_val_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(config_->save_interval_msec);
    if (config_->is_output_console) {
        outputs_.emplace_back(std::make_shared<LoggerOutputToConsole>());
    }

    if (config_->is_output_file) {
        std::shared_ptr<LoggerOutputToFile> output_file = std::make_shared<LoggerOutputToFile>(config_);
        if (output_file->CheckAndCreateLogDir(config_->logger_dir_path)) {
            output_file->SetFileNameTag(file_name_tag);
            outputs_.emplace_back(std::move(output_file));
        }
        else {
            std::cerr << "fail to do logger_dir_path:" << config_->logger_dir_path << std::endl;
        }
    }
}

void LoggerObject::SetLoggerOutput(std::shared_ptr<LoggerOutput>&& output) {
    outputs_.emplace_back(output);
}

void LoggerObject::CheckFileAndDisk(const std::chrono::steady_clock::time_point& cur_time) {
    if (check_time_val_ > cur_time) {
        return;
    }
    static int kCheckDiskCounter = 0;
    check_time_val_ = cur_time + std::chrono::milliseconds(2000);
    for (auto& it : outputs_) {
        it->CheckFiles();
    }
    if (++kCheckDiskCounter > 30) {
        for (auto& it : outputs_) {
            it->CheckDisk();
        }
        kCheckDiskCounter = 0;
    }
}

void LoggerObject::Flush() {
    for (auto& it : outputs_) {
         it->Flush();
    }
}

bool LoggerObject::Sync(const std::chrono::steady_clock::time_point& cur_time) {
    if (sysc_time_val_ > cur_time) {
        return false;
    }
    sysc_time_val_ = cur_time + std::chrono::milliseconds(config_->save_interval_msec);
    for (auto& it : outputs_) {
         it->Sync();
    }
    return true;
}

void* LoggerObject::PreviousCheck(std::shared_ptr<LoggerData>& logger_data) {
    for (auto& it : outputs_) {
        auto val = it->PreviousCheck(logger_data);
        if (val) {
            return val;
        }
    }
    return nullptr;
}

void LoggerObject::Write(std::shared_ptr<LoggerData>& logger_data) {
    for (auto& it : outputs_) {
        it->OnLoggerData(logger_data);
    }
    logger_data->logger_buffer->set_write_index(0);
}

} // namespace logger
} // namespace agile