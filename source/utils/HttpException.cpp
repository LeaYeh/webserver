#include "HttpException.hpp"
#include "Logger.hpp"
#include "defines.hpp"
#include "shellUtils.hpp"

namespace utils
{

HttpException::HttpException(const webshell::StatusCode& status_code,
                             const std::string& reasonDetail,
                             webshell::ContentType content_type) :
    _statusCode(status_code),
    _reasonDetail(reasonDetail),
    _content_type(content_type)
{
    weblog::Logger::log(weblog::CRITICAL,
                        "HttpException::HttpException content_type: "
                            + webshell::content_type_to_string(content_type));
}

HttpException::HttpException(const HttpException& other) :
    _statusCode(other._statusCode),
    _reasonDetail(other._reasonDetail),
    _content_type(other._content_type)
{
    weblog::Logger::log(
        weblog::CRITICAL,
        "HttpException::HttpException other content_type: "
            + webshell::content_type_to_string(other._content_type));
}

HttpException& HttpException::operator=(const HttpException& other)
{
    if (this != &other) {
        _statusCode = other._statusCode;
        _reasonDetail = other._reasonDetail;
        _content_type = other._content_type;
    }
    weblog::Logger::log(
        weblog::CRITICAL,
        "HttpException::operator= other content_type: "
            + webshell::content_type_to_string(other._content_type));
    return (*this);
}

HttpException::~HttpException() throw() {}

const char* HttpException::what() const throw()
{
    if (_reasonDetail.empty()) {
        _reasonDetail = webshell::status_reason_phase(_statusCode);
    }
    _reasonDetail += "\n";
    return (_reasonDetail.c_str());
}

webshell::StatusCode HttpException::status_code() const
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
