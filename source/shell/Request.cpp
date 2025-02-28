#include "Request.hpp"
#include "Config.hpp"
#include "ConfigHttpBlock.hpp"
#include "ConfigLocationBlock.hpp"
#include "ConnectionHandler.hpp"
#include "HttpException.hpp"
#include "Uri.hpp"
#include "defines.hpp"
#include "shellUtils.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdlib>
#include <ios>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/stat.h>

namespace webshell
{

Request::Request() :
    _processed(0),
    _state(CHUNKSIZE),
    _size_num(-1),
    _method(UNKNOWN),
    _uri(),
    _version(),
    _headers(),
    _cookies(),
    _read_buffer()
{
}

Request::Request(std::string* buffer) :
    _processed(0),
    _state(CHUNKSIZE),
    _size_num(-1),
    _method(UNKNOWN),
    _uri(),
    _version(),
    _headers(),
    _cookies(),
    _read_buffer(buffer)
{
}

Request::Request(const Request& other) :
    _processed(other._processed),
    _state(other._state),
    _size_num(other._size_num),
    _method(other._method),
    _uri(other._uri),
    _version(other._version),
    _headers(other._headers),
    _cookies(other._cookies),
    _read_buffer(other._read_buffer),
    _config(other._config)
{
}

Request& Request::operator=(const Request& other)
{
    if (this != &other) {
        _processed = other._processed;
        _state = other._state;
        _size_num = other._size_num;
        _method = other._method;
        _uri = other._uri;
        _version = other._version;
        _headers = other._headers;
        _cookies = other._cookies;
        _read_buffer = other._read_buffer;
        _config = other._config;
    }
    return (*this);
}

Request::~Request() {}

const RequestMethod& Request::method() const
{
    return (_method);
}

Uri Request::uri() const
{
    return (_uri);
}

float Request::version() const
{
    return (_version);
}

const webconfig::RequestConfig& Request::config() const
{
    return (_config);
}

const std::map<std::string, std::string>& Request::headers() const
{
    return (_headers);
}

const std::map<std::string, std::string>& Request::cookies() const
{
    return (_cookies);
}

const std::string& Request::get_header(const std::string& name) const
{
    std::map<std::string, std::string>::const_iterator it = _headers.find(name);

    if (it == _headers.end()) {
        return (utils::EMPTY_STRING);
    }
    return (it->second);
}

const std::string& Request::get_cookie(const std::string& name) const
{
    std::map<std::string, std::string>::const_iterator it =
        _cookies.find(name);

    if (it == _cookies.end()) {
        return (utils::EMPTY_STRING);
    }
    return (it->second);
}

bool Request::has_header(const std::string& name) const
{
    return (_headers.find(name) != _headers.end());
}

bool Request::has_cookie(const std::string& name) const
{
    return (_cookies.find(name) != _cookies.end());
}

bool Request::has_cookies() const
{
    return (!_cookies.empty());
}

const std::string Request::serialize() const
{
    std::string serialized;

    // serialize request line
    serialized += request_method_to_string(_method) + " " + _uri.raw + " HTTP/"
                  + utils::to_string(_version) + "\r\n";
    // serialize headers
    for (std::map<std::string, std::string>::const_iterator it =
             _headers.begin();
         it != _headers.end();
         ++it) {
        serialized += it->first + ": " + it->second + "\r\n";
    }
    // serialize body
    serialized += "\r\n";

    return (serialized);
}

void Request::set_method(RequestMethod method)
{
    _method = method;
}

void Request::set_cookies(std::map<std::string, std::string> cookies)
{
    _cookies = cookies;
}

void Request::set_headers(std::map<std::string, std::string> headers)
{
    _headers = headers;
}

void Request::set_uri(Uri uri)
{
    _uri = uri;
}

void Request::set_version(float version)
{
    _version = version;
}

void Request::set_reference(std::string* read_buffer)
{
    _read_buffer = read_buffer;
}

void Request::add_header(std::string& name, std::string& value)
{
    _headers[name] = value;
}

bool Request::empty_buffer() const
{
    return ((*_read_buffer).empty());
}

std::string Request::read_chunked_body()
{
    static bool chunked = (_headers.find("content-length") == _headers.end());
    std::vector<char> chunked_body;
    bool is_eof;

    if (!chunked) {
        is_eof = _proceed_content_len(chunked_body);
    }
    else {
        is_eof = _proceed_chunked(chunked_body);
    }
    _write_chunked_file(chunked_body);
    if (is_eof)
    {
        return _uploader.temp_filename();
    }
    else
        return "";
}

const UploadRecord2& Request::uploader() const
{
    return (_uploader);
}

bool Request::read_chunked_body(std::vector<char>& chunked_body)
{
    static bool chunked = (_headers.find("content-length") == _headers.end());

    if (!chunked) {
        return (_proceed_content_len(chunked_body));
    }
    else {
        return (_proceed_chunked(chunked_body));
    }
}

void Request::_write_chunked_file(const std::vector<char>& content)
{
    std::ofstream& file_stream = _uploader.file_stream;
    size_t remaining = content.size();
    size_t offset = 0;

    if (!file_stream.is_open()) {
        throw utils::HttpException(webshell::INTERNAL_SERVER_ERROR,
                                   "File stream is not open");
    }
    while (remaining > 0) {
        size_t write_size =
            std::min(remaining, (size_t)webkernel::CHUNKED_SIZE);
        file_stream.write(content.data() + offset, write_size);
        if (!file_stream.good()) {
            throw utils::HttpException(
                webshell::INTERNAL_SERVER_ERROR,
                "Write file failed: " + utils::to_string(std::strerror(errno)));
        }
        offset += write_size;
        remaining -= write_size;
    }
    file_stream.flush();
}

/*
TODO
Any Content-Length field value greater than or equal to zero is
valid.  Since there is no predefined limit to the length of a
payload, a recipient MUST anticipate potentially large decimal
numerals and prevent parsing errors due to integer conversion
overflows (RFC 7230).
*/
bool Request::_proceed_content_len(std::vector<char>& chunked_body)
{
    static size_t chunksize = webkernel::CHUNKED_SIZE;
    size_t payload = atoi(_headers["content-length"].c_str());
    size_t max_payload = _config.client_max_body_size;
    size_t buffer_size = (*_read_buffer).size();

    if (payload > max_payload) {
        throw utils::HttpException(webshell::PAYLOAD_TOO_LARGE,
                                   "Data size exceeds client_max_body_size 2",
                                   webshell::TEXT_PLAIN);
    }

    if (buffer_size < chunksize) {
        if (payload - _processed > buffer_size) {
            chunked_body = std::vector<char>(
                _read_buffer->begin(), _read_buffer->begin() + buffer_size);
            _processed += buffer_size;
            _read_buffer->clear();
            return (false);
        }
        chunked_body =
            std::vector<char>(_read_buffer->begin(),
                              _read_buffer->begin() + payload - _processed);
        _read_buffer->erase(0, payload - _processed);
        _processed = 0;
        return (true);
    }
    else {
        if (payload - _processed > chunksize) {
            chunked_body = std::vector<char>(_read_buffer->begin(),
                                             _read_buffer->begin() + chunksize);
            (*_read_buffer).erase(0, chunksize);
            _processed += chunksize;
            return (false);
        }
        chunked_body =
            std::vector<char>(_read_buffer->begin(),
                              _read_buffer->begin() + payload - _processed);
        _read_buffer->erase(0, payload - _processed);
        _processed = 0;
        return (true);
    }
}

bool Request::_proceed_chunked(std::vector<char>& chunked_body)
{
    static size_t max_payload = _config.client_max_body_size;

    chunked_body.clear();
    if (_processed > max_payload) {
        throw utils::HttpException(webshell::PAYLOAD_TOO_LARGE,
                                   "Payload exceeding client max body size");
    }
    for (size_t idx = 0; idx < _read_buffer->size(); idx++) {
        unsigned char c = (*_read_buffer)[idx];
        switch (_state) {
        case CHUNKSIZE:
            _check_hexdigit(c);
            break;
        case SIZE_CRLF:
            _check_size_crlf(c);
            break;
        case CHUNKBODY:
            _check_body(c);
            break;
        case BODY_CRLF: {
            _check_body_crlf(c);
            chunked_body = _chunkbuf;
            (*_read_buffer) = _read_buffer->substr(idx + 1);
            _chunksize.clear();
            if (_chunkbuf.empty()) {
                return (true);
            }
            _processed += _chunkbuf.size();
            _chunkbuf.clear();
            return (false);
        }
        default:
            throw std::runtime_error("wtf");
        }
        if (_chunkbuf.size() > webkernel::BUFFER_SIZE * 2) {
            throw utils::HttpException(
                webshell::PAYLOAD_TOO_LARGE,
                "Chunk size cannot be bigger than "
                    + utils::to_string(webkernel::BUFFER_SIZE * 2),
                webshell::TEXT_PLAIN);
        }
    }
    _read_buffer->clear();
    return (false);
}

void Request::setup_config(webconfig::ConfigServerBlock* server_config)
{
    webconfig::ConfigLocationBlock* location_config = NULL;
    webconfig::Config* config = webconfig::Config::instance();

    // std::cout << config << std::endl;
    webconfig::ConfigHttpBlock http_config = config->http_block();

    for (std::size_t i = 0; i < server_config->location_block_list().size();
         i++) {
        if (utils::start_with(
                _uri.path, server_config->location_block_list()[i].route())) {
            location_config = &server_config->location_block_list()[i];
            break;
        }
    }
    if (location_config == NULL) {
        throw utils::HttpException(webshell::NOT_FOUND,
                                   "No matching location block found: "
                                       + _uri.path,
                                   webshell::TEXT_PLAIN);
    }

    _config = webconfig::RequestConfig();

    _config.client_max_body_size = http_config.client_max_body_size();
    _config.default_type = http_config.default_type();
    _config.autoindex_page = http_config.autoindex_page();
    _config.error_log = server_config->error_log();
    _config.keep_alive_timeout = server_config->keep_alive_timeout();
    _config.server_name = server_config->server_name();

    _config.route = location_config->route();
    _config.limit_except = location_config->limit_except();
    _config.root = location_config->root();
    _config.index = location_config->index();
    _config.redirect = location_config->redirect();
    _config.autoindex = location_config->autoindex();
    _config.cgi_extension = location_config->cgi_extension();
    _config.cgi_path = location_config->cgi_path();
    _config.enable_upload = location_config->enable_upload();
    _config.upload_path = location_config->upload_path();
}

void Request::_check_hexdigit(unsigned char c)
{
    if (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
        _chunksize.push_back(c);
        return;
    }
    else if (c == '\r') {
        std::istringstream(_chunksize) >> std::hex >> _size_num;
        if (_size_num > webkernel::BUFFER_SIZE * 2) {
            throw utils::HttpException(
                webshell::PAYLOAD_TOO_LARGE,
                "Chunk size cannot be bigger than "
                    + utils::to_string(webkernel::BUFFER_SIZE * 2),
                webshell::TEXT_PLAIN);
        }
        _state = SIZE_CRLF;
        return;
    }
    throw utils::HttpException(webshell::BAD_REQUEST,
                               "Malformed chunk size: should be hexdigit",
                               webshell::TEXT_PLAIN);
}

void Request::_check_size_crlf(unsigned char c)
{
    if (c == '\n') {
        _state = CHUNKBODY;
    }
    else {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "Malformed chunk size: should end with CRLF",
                                   webshell::TEXT_PLAIN);
    }
}

void Request::_check_body(unsigned char c)
{
    if (_size_num > 0) {
        _chunkbuf.push_back(c);
        _size_num--;
    }
    else if (_size_num == 0 && c == '\r') {
        _state = BODY_CRLF;
    }
    else {
        std::cout << "SIZE: " << _size_num << " CHAR: |" << c << "|"
                  << std::endl;
        throw utils::HttpException(
            webshell::BAD_REQUEST,
            "Malformed chunk size: not matching chunk length",
            webshell::TEXT_PLAIN);
    }
}

void Request::_check_body_crlf(unsigned char c)
{
    if (c == '\n') {
        _size_num = -1;
        _state = CHUNKSIZE;
    }
    else {
        throw utils::HttpException(webshell::BAD_REQUEST,
                                   "Chunk should end with CRLF",
                                   webshell::TEXT_PLAIN);
    }
}

} // namespace webshell
