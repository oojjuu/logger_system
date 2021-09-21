#include "LoggerObject.h"

#include <assert.h>

#include "LoggerBufferImp.h"
#include "LoggerConfigManager.h"
#include "LoggerOutputToConsole.h"
#include "LoggerOutputToFile.h"

namespace agile {
namespace logger {

LoggerObject::LoggerObject(int conf_id)
{
    config_ = LoggerConfigManager::GetInstance().GetConfig(conf_id);
    if (config_->is_output_console)
    {
        outputs_.push_back(std::make_shared<LoggerOutputToConsole>());
    }
    if (config_->is_output_file)
	{
        outputs_.push_back(std::make_shared<LoggerOutputToFile>(config_));
    }
    //cur_thread_id_ = TlsGetTid();
    
    for (uint16_t i = 0; i < config_->logger_cached_size; i++)
    {
       	std::shared_ptr<LoggerData> new_val = std::make_shared<LoggerData>();
		new_val->logger_buffer = std::make_shared<LoggerBufferImp>(config_);
        datas_.emplace_back(std::move(new_val));
    }
}

std::shared_ptr<LoggerData> LoggerObject::PopData()
{
    if (!config_->is_single_thread_logger)
    {
        std::lock_guard<std::mutex> lg(logger_mtx_);
        return PopDataWithoutLock();
    }

    assert (config_->is_single_thread_logger == !is_mul_threads_);
    return PopDataWithoutLock();
}

std::shared_ptr<LoggerData> LoggerObject::PopDataWithoutLock()
{
    if (datas_.empty())
    {
       	std::shared_ptr<LoggerData> new_val = std::make_shared<LoggerData>();
		new_val->logger_buffer = std::make_shared<LoggerBufferImp>(config_);
        return std::move(new_val);
    }

	std::shared_ptr<LoggerData> val = datas_.front();
    datas_.pop_front();
    return std::move(val);
}

void LoggerObject::PushData(std::shared_ptr<LoggerData>&& data)
{
    if (!config_->is_single_thread_logger)
    {
        std::lock_guard<std::mutex> lg(logger_mtx_);
        PushDataWithoutLock(std::move(data));
        return;
    }
    PushDataWithoutLock(std::move(data));
}

void LoggerObject::PushDataWithoutLock(std::shared_ptr<LoggerData>&& data)
{
    if (config_->save_in_real_time)
    {
        std::shared_ptr<LoggerData> temp = std::move(data);
        for(auto& it : outputs_)
        {
            it->OnLoggerData(temp);
        }
        temp->logger_buffer->SetWriteIndex(0);
        datas_.emplace_back(std::move(temp));
    }
    else
    {
        waiting_datas_.emplace_back(std::move(data));
        if (waiting_datas_.size() >= config_->save_on_max_logger_num)
        {
            FlushWithoutLock();
        }
    }
}

void LoggerObject::Flush()
{
    if (!config_->is_single_thread_logger)
    {
        std::lock_guard<std::mutex> lg(logger_mtx_);
        FlushWithoutLock();
        return;
    }
    FlushWithoutLock();
}

void LoggerObject::FlushWithoutLock()
{
    if (!waiting_datas_.empty())
    {
        for(auto& it : outputs_)
        {
            it->OnLoggerDatas(waiting_datas_);
        }
        while (!waiting_datas_.empty())
        {
            std::shared_ptr<LoggerData>& temp = waiting_datas_.front();
            temp->logger_buffer->SetWriteIndex(0);
            datas_.emplace_back(std::move(waiting_datas_.front()));
            waiting_datas_.pop_front();
        }
    }
}

} // namespace logger
} // namespace agile