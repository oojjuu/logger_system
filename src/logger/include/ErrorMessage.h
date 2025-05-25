#ifndef AGILE_ERROR_MESSAGE_H
#define AGILE_ERROR_MESSAGE_H

#include <string>

namespace agile {
namespace logger {
class ErrorMessage final {
public:
    ErrorMessage() = default;
	~ErrorMessage() = default;
    int32_t GetErrno() const { return errno_; }
    void SaveErrno();
    std::string ToString() const;
    static std::string ToString(int32_t err);

private:
    int32_t errno_ = 0;
};
} // namespace logger
} // namespace agile
#endif // AGILE_ERROR_MESSAGE_H
