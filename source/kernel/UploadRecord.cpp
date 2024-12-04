#include "UploadRecord.hpp"
#include "utils.hpp"
#include "kernelUtils.hpp"
#include <cstdio>

namespace webkernel
{

UploadRecord::UploadRecord()
    : _total_size(0), _uploaded_size(0), _start_time(time(NULL)),
      _state(UPLOAD_INIT)
{
    _temp_filename = _generate_temp_file_path("");
    _file_stream.open(_temp_filename.c_str(), std::ios::out | std::ios::binary);
    if (!_file_stream.is_open())
        throw std::runtime_error("UploadRecord: failed to open file stream");
}

UploadRecord::UploadRecord(const std::string& target_filename)
    : _total_size(0), _uploaded_size(0), _start_time(time(NULL)),
      _target_filename(target_filename), _state(UPLOAD_INIT)
{
    _temp_filename =
        _generate_temp_file_path(utils::basefolder(target_filename));
    _file_stream.open(_temp_filename.c_str(), std::ios::out | std::ios::binary);
    if (!_file_stream.is_open())
        throw std::runtime_error("UploadRecord: failed to open file stream");
}

UploadRecord::UploadRecord(const UploadRecord& other)
    : _total_size(other._total_size), _uploaded_size(other._uploaded_size),
      _start_time(other._start_time), _target_filename(other._target_filename),
      _temp_filename(other._temp_filename), _state(other._state)
{
    _file_stream.open(_temp_filename.c_str(), std::ios::out | std::ios::binary);
    if (!_file_stream.is_open())
        throw std::runtime_error("UploadRecord: failed to open file stream");
}

UploadRecord& UploadRecord::operator=(const UploadRecord& other)
{
    if (this != &other)
    {
        _total_size = other._total_size;
        _uploaded_size = other._uploaded_size;
        _start_time = other._start_time;
        _target_filename = other._target_filename;
        _temp_filename = other._temp_filename;
        _state = other._state;
        _file_stream.open(_temp_filename.c_str(), std::ios::out | std::ios::binary);
        if (!_file_stream.is_open())
            throw std::runtime_error("UploadRecord: failed to open file stream");
    }
    return (*this);
}

UploadRecord::~UploadRecord()
{
    _close_file_stream();
    if (access(_temp_filename.c_str(), F_OK) == 0)
        unlink(_temp_filename.c_str());
}

void UploadRecord::update()
{
    if (_uploaded_size == _total_size)
    {
        _state = UPLOAD_COMPLETE;
        if (rename(_temp_filename.c_str(), _target_filename.c_str()) == -1)
            throw std::runtime_error("UploadRecord: failed to rename file");
    }
    struct stat file_stat;
    if (stat(_temp_filename.c_str(), &file_stat) == -1)
        _uploaded_size = 0;
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

std::ofstream& UploadRecord::file_stream()
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
    serialized += "}\n";

    return (serialized);
}

std::string UploadRecord::_generate_temp_file_path(std::string folder)
{
    if (folder.empty())
        folder = "/tmp";
    else if (folder[folder.size() - 1] != '/')
        folder += "/";
    std::string temp_file_path = folder + uuid();

    return (temp_file_path);
}

void UploadRecord::_open_file_stream(const std::string& file_path)
{
    _file_stream.open(file_path.c_str(), std::ios::out | std::ios::binary);
    if (!_file_stream.is_open())
        throw std::runtime_error("UploadRecord: failed to open file stream");
}

void UploadRecord::_close_file_stream()
{
    if (_file_stream.is_open())
        _file_stream.close();
}

} // namespace webkernel
