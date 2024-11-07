#include "HttpException.hpp"
#include "defines.hpp"
#include "shellUtils.hpp"
#include "utils.hpp"
#include "Logger.hpp"

namespace utils
{

HttpException::HttpException(const webshell::StatusCode& statusCode,
                             const std::string& reasonDetail)
    : _statusCode(statusCode), _reasonDetail(reasonDetail)
{
}

HttpException::~HttpException() throw()
{
}

const char* HttpException::what() const throw()
{
    if (_reasonDetail.empty())
        _reasonDetail = webshell::statusReasonPhase(_statusCode);
    return (_reasonDetail.c_str());
}

webshell::StatusCode HttpException::statusCode() const
{
    return (_statusCode);
}

std::string HttpException::reasonDetail() const
{
    return (_reasonDetail);
}

} // namespace utils
