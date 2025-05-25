#ifndef AGILE_LOGGER_LOGGER_WRITE_INTERFACE_H
#define AGILE_LOGGER_LOGGER_WRITE_INTERFACE_H

#include "LoggerData.h"

namespace agile {
namespace logger {
class LoggerWriteInterface {
public:
	LoggerWriteInterface() = default;
	virtual ~LoggerWriteInterface() = default;
	virtual void Write(const std::shared_ptr<LoggerData>& loggerData) = 0;
};
} // namespace logger
} // namespace agile
#endif // AGILE_LOGGER_LOGGER_WRITE_INTERFACE_H
