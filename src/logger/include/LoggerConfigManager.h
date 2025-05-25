#ifndef AGILE_LOGGER_LOGGER_CONFIG_MANAGER_H
#define AGILE_LOGGER_LOGGER_CONFIG_MANAGER_H

#include <assert.h>
#include <vector>
#include "LoggerConfig.h"

namespace agile {
namespace logger {
/**
*@brief logger configure manager class
*/
class LoggerConfigManager {
public:
    static LoggerConfigManager& GetInstance()
    {
        static LoggerConfigManager instance;
        return instance;
    }
    ~LoggerConfigManager() = default;
    bool Init(const std::string& configFilePath);

    const LoggerConfig* GetConfig(uint8_t confId) const;
    const std::vector<LoggerConfig>& configs() const { return configs_; }

private:
    LoggerConfigManager();
    LoggerConfig MakeDefaultGetConfig() const;

    bool ReadFromFileData(const std::string &data);
    bool ReadConfigData(const std::string& data, uint32_t confId, uint32_t& offset);

    bool IsExistLoggerName(const std::string& dirName, const std::string& loggerName);

    bool GetKeyValue(const std::string& data, uint32_t offset, uint32_t& maxOffset,
        const std::string& key, std::string& val);
    void AutoGetBoolValue(const std::string& data, uint32_t offset, uint32_t& maxOffset,
        const std::string& key, bool& val);
    bool AutoGetIntValue(const std::string& data, uint32_t offset, uint32_t& maxOffset,
        const std::string& key, uint64_t& val, uint64_t min, uint64_t max);
    bool AutoGetIntValue(const std::string& data, uint32_t offset, uint32_t& maxOffset,
        const std::string& key, uint32_t& val, uint32_t min, uint32_t max);

    bool inited_ = false;
    std::vector<LoggerConfig> configs_;
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_CONFIG_MANAGER_H
