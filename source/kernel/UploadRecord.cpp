#include "UploadRecord.hpp"
#include "Logger.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils.hpp"
#include <cstdio>
#include <iostream>
#include <unistd.h>

namespace webkernel
{

UploadRecord::UploadRecord()
    : _total_size(0), _uploaded_size(0), _start_time(time(NULL)),
      _target_filename(""), _temp_filename(""), _state(UPLOAD_INIT),
      _file_stream(NULL)
{
    weblog::Logger::log(weblog::WARNING, "UploadRecord: constructor address: " +
                                             utils::toString((size_t)this));
}

UploadRecord::UploadRecord(const std::string& target_filename,
                           size_t total_size)
    : _total_size(total_size), _uploaded_size(0), _start_time(time(NULL)),
      _target_filename(target_filename), _temp_filename(""),
      _state(UPLOAD_INIT), _file_stream(NULL)
{
    _temp_filename =
        _generate_temp_file_path(utils::basefolder(target_filename));
    _file_stream = new std::ofstream();
    _file_stream->open(_temp_filename.c_str(),
                       std::ios::out | std::ios::binary);
    if (!_file_stream->is_open() || !_file_stream->good())
        throw std::runtime_error("UploadRecord: failed to open file stream");
    weblog::Logger::log(weblog::WARNING,
                        "UploadRecord: constructor: " + _temp_filename + " " +
                            _target_filename +
                            " address: " + utils::toString((size_t)this));
}

UploadRecord::~UploadRecord()
{
    weblog::Logger::log(weblog::WARNING,
                        "UploadRecord: destructor: " + _temp_filename + ", " +
                            _target_filename +
                            " address: " + utils::toString((size_t)this));
    _close_file_stream();
    if (access(_temp_filename.c_str(), F_OK) == 0)
        unlink(_temp_filename.c_str());
}

void UploadRecord::update(bool is_last_chunk)
{
    if (_target_filename.empty())
        throw std::runtime_error("UploadRecord: target filename is empty");
    if (is_last_chunk)
    {
        _state = UPLOAD_COMPLETE;
        unlink(_target_filename.c_str());
        if (rename(_temp_filename.c_str(), _target_filename.c_str()) == -1)
            throw std::runtime_error(
                "UploadRecord: failed to rename file from " + _temp_filename +
                " to " + _target_filename + " " + strerror(errno));
    }
    static struct stat file_stat;

    if (stat(_temp_filename.c_str(), &file_stat) == -1)
        _uploaded_size = 0;
    else
        _uploaded_size = file_stat.st_size;
}

bool UploadRecord::success() const
{
    return (_state == UPLOAD_COMPLETE);
}

float UploadRecord::upload_speed() const
{
    time_t now = time(NULL);
    float speed = (_uploaded_size * 1.0) / (now - _start_time + 1);
    return (speed);
}

time_t UploadRecord::upload_time() const
{
    time_t now = time(NULL);
    return (now - _start_time);
}

float UploadRecord::upload_progress() const
{
    if (_total_size == 0)
        return (1.0);
    float progress_percent = (_uploaded_size * 1.0) / _total_size;
    return (progress_percent);
}

size_t UploadRecord::total_size() const
{
    return (_total_size);
}

size_t UploadRecord::uploaded_size() const
{
    return (_uploaded_size);
}

time_t UploadRecord::start_time() const
{
    return (_start_time);
}

std::string UploadRecord::target_filename() const
{
    return (_target_filename);
}

std::ofstream* UploadRecord::file_stream()
{
    return (_file_stream);
}

std::string UploadRecord::serialize() const
{
    std::string serialized;

    serialized += "{\n";
    serialized += "  \"total_size\": " + utils::toString(_total_size) + ",\n";
    serialized +=
        "  \"uploaded_size\": " + utils::toString(_uploaded_size) + ",\n";
    serialized += "  \"start_time\": " + utils::toString(_start_time) + ",\n";
    serialized += "  \"filename\": \"" + _target_filename + "\"\n";
    serialized +=
        "  \"upload_time\": " + utils::toString(upload_time()) + ",\n";
    serialized +=
        "  \"upload_speed\": " + utils::toString(upload_speed()) + ",\n";
    serialized +=
        "  \"upload_progress\": " + utils::toString(upload_progress()) + ",\n";
    serialized += "  \"success\": " + utils::toString(success()) + "\n";
    serialized += "}\n";

    return (serialized);
}

std::string UploadRecord::_generate_temp_file_path(std::string folder)
{
    if (folder.empty())
        folder = "/tmp";
    if (folder[folder.size() - 1] != '/')
        folder += "/";
    std::string temp_file_path = folder + uuid();

    return (temp_file_path);
}

void UploadRecord::_open_file_stream(const std::string& file_path)
{
    _file_stream->open(file_path.c_str(), std::ios::out | std::ios::binary);
    if (!_file_stream->is_open())
        throw std::runtime_error("UploadRecord: failed to open file stream");
}

void UploadRecord::_close_file_stream()
{
    if (_file_stream)
    {
        if (_file_stream->is_open())
            _file_stream->close();
        delete _file_stream;
        _file_stream = NULL;
    }
}

} // namespace webkernel
