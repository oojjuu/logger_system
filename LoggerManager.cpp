#include "LoggerManager.h"

#include <assert.h>
#include <iostream>

#include "Logger.h"
#include "LoggerConfigManager.h"

namespace agile {
namespace logger {

bool InitLogger(const std::string& config_file_path)
{
    bool ret = LoggerConfigManager::GetInstance().Init(config_file_path);
	if (ret)
	{
		LoggerManager::GetInstance().Init();
	}
	return ret;
}

LoggerManager::LoggerManager()
{
	
}

void LoggerManager::Init()
{
	const std::vector<LoggerConfig>& configs = LoggerConfigManager::GetInstance().GetConfigs();
	for(auto& it : configs)
	{
		datas_.emplace_back(std::make_shared<LoggerObject>(it.conf_id));
	}
	for(auto& it : datas_)
	{
		it->TlsGetTid();
	}
}

void LoggerManager::Flush()
{
	for(auto& it : datas_)
	{
		it->Flush();
	}
}

void LoggerManager::Flush(uint32_t conf_id)
{
	assert( conf_id < datas_.size() );
	return datas_[conf_id]->Flush();
}

int LoggerManager::TlsGetTid(uint32_t conf_id)
{
	// std::cout << "conf_id:" << conf_id << ", size:" << datas_.size() << std::endl;
	assert( conf_id < datas_.size() );
	return datas_[conf_id]->TlsGetTid();
}

std::shared_ptr<LoggerData> LoggerManager::PopData(uint32_t conf_id)
{
	assert( conf_id < datas_.size() );
	return datas_[conf_id]->PopData();
}	

void LoggerManager::PushData(std::shared_ptr<LoggerData>&& data)
{
	assert( data->conf_id < datas_.size() );
	datas_[data->conf_id]->PushData(std::move(data));
}

} // namespace logger
} // namespace agile