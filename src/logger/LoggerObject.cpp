#include "LoggerObject.h"

#include <assert.h>
#include "LoggerConfigManager.h"
#include "LoggerOutputToConsole.h"
#include "LoggerOutputToFile.h"
#include "LoggerUtil.h"

namespace agile {
namespace logger {

LoggerObject::LoggerObject(int conf_id, const std::string& file_name_tag) {
    config_ = LoggerConfigManager::GetInstance().GetConfig(conf_id);
    sysc_time_val_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(config_->save_interval_msec);
    if (config_->is_output_console) {
        outputs_.emplace_back(std::make_shared<LoggerOutputToConsole>());
    }

    if (config_->is_output_file) {
        std::shared_ptr<LoggerOutputToFile> output_file = std::make_shared<LoggerOutputToFile>(file_name_tag, config_);
        if (LoggerUtil::CheckAndCreateLogDir(config_->logger_dir_path)) {
            outputs_.emplace_back(std::move(output_file));
        }
        else {
            std::cerr << "fail to do logger_dir_path:" << config_->logger_dir_path << std::endl;
        }
    }
}

void LoggerObject::Run(const std::chrono::steady_clock::time_point& cur_time) {
    for (auto& it : outputs_) {
         it->Run(cur_time);
    }
}

void LoggerObject::PreviousCheck(const std::shared_ptr<LoggerData>& logger_data) {
    for (auto& it : outputs_) {
        it->PreviousCheck(logger_data);
    }
}

void LoggerObject::Write(const std::shared_ptr<LoggerData>& logger_data) {
    for (auto& it : outputs_) {
        it->OnLoggerData(logger_data);
    }
    logger_data->logger_buffer->set_write_index(0);
}

} // namespace logger
} // namespace agile