#include "HttpException.hpp"
#include "defines.hpp"
#include "shellUtils.hpp"

namespace utils
{

HttpException::HttpException(const webshell::StatusCode& statusCode,
                             const std::string& reasonDetail,
                             webshell::ContentType content_type)
    : _statusCode(statusCode), _reasonDetail(reasonDetail),
      _content_type(content_type)
{
}

HttpException::~HttpException() throw()
{
}

const char* HttpException::what() const throw()
{
    if (_reasonDetail.empty())
        _reasonDetail = webshell::statusReasonPhase(_statusCode);
    _reasonDetail += "\n";
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

webshell::ContentType HttpException::contentType() const
{
    return (_content_type);
}

} // namespace utils
