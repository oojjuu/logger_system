#include "ErrorMessage.h"

#include <cerrno>
#include <sstream>

namespace agile {
namespace logger {
void ErrorMessage::SaveErrno()
{
    errno_ = errno;
}

std::string ErrorMessage::ToString() const
{
    return ToString(errno_);
}

std::string ErrorMessage::ToString(int32_t err)
{
    std::stringstream ss;
    ss << " (errno" << err << " : " << strerror(err) << ") ";
    return ss.str();
}
} // logger
} // agile