#include "LoggerObject.h"

#include "LoggerConfigManager.h"
#include "LoggerOutputToConsole.h"
#include "LoggerOutputToFile.h"
#include "LoggerUtil.h"

namespace agile {
namespace logger {

LoggerObject::LoggerObject(int conf_id, const std::string& file_name_tag) {
    config_ = LoggerConfigManager::GetInstance().GetConfig(conf_id);
    if (config_->is_output_console) {
        outputs_.emplace_back(std::make_shared<LoggerOutputToConsole>());
    }

    if (config_->is_output_file) {
        if (LoggerUtil::CheckAndCreateLogDir(config_->logger_dir_path)) {
            outputs_.emplace_back(std::make_shared<LoggerOutputToFile>(file_name_tag, config_));
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
}

} // namespace logger
} // namespace agile