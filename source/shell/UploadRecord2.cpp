#include "UploadRecord2.hpp"
#include "Logger.hpp"
#include "defines.hpp"
#include "kernelUtils.hpp"
#include "utils.hpp"
#include <cstdio>
#include <iostream>
#include <unistd.h>
// #include <unistd.h>

namespace webshell
{

UploadRecord2::UploadRecord2() :
    _total_size(0),
    _uploaded_size(0),
    _start_time(time(NULL)),
    _temp_filename(""),
    _state(UPLOAD_INIT)
{
    LOG(weblog::WARNING,
        "UploadRecord2: constructor address: " + utils::to_string((size_t)this));
    _temp_filename =
        _generate_temp_file_path();
    _open_file_stream(_temp_filename);
}

UploadRecord2::UploadRecord2(size_t total_size) :
    _total_size(total_size),
    _uploaded_size(0),
    _start_time(time(NULL)),
    _temp_filename(""),
    _state(UPLOAD_INIT)
{
    _temp_filename =
        _generate_temp_file_path();
    _open_file_stream(_temp_filename);
}

UploadRecord2::~UploadRecord2()
{
    LOG(weblog::WARNING,
        "UploadRecord2: destructor: " + _temp_filename
            + " address: " + utils::to_string((size_t)this));
    if (access(_temp_filename.c_str(), F_OK) == 0
        && std::remove(_temp_filename.c_str()) == -1) {
        throw std::runtime_error("UploadRecord2: failed to remove temp file "
                                 + _temp_filename);
    }
}

void UploadRecord2::update(bool is_last_chunk)
{
    if (is_last_chunk) {
        _state = UPLOAD_COMPLETE;
    }
    static struct stat file_stat;

    if (stat(_temp_filename.c_str(), &file_stat) == -1) {
        _uploaded_size = 0;
    }
    else {
        _uploaded_size = file_stat.st_size;
    }
}

bool UploadRecord2::success() const
{
    return (_state == UPLOAD_COMPLETE);
}

float UploadRecord2::upload_speed() const
{
    time_t now = time(NULL);
    float speed = (_uploaded_size * 1.0) / (now - _start_time + 1);
    return (speed);
}

time_t UploadRecord2::upload_time() const
{
    time_t now = time(NULL);
    return (now - _start_time);
}

float UploadRecord2::upload_progress() const
{
    if (_total_size == 0) {
        return (1.0);
    }
    float progress_percent = (_uploaded_size * 1.0) / _total_size;
    return (progress_percent);
}

size_t UploadRecord2::total_size() const
{
    return (_total_size);
}

size_t UploadRecord2::uploaded_size() const
{
    return (_uploaded_size);
}

time_t UploadRecord2::start_time() const
{
    return (_start_time);
}

std::string UploadRecord2::temp_filename() const
{
    return (_temp_filename);
}

std::string UploadRecord2::serialize() const
{
    std::string serialized;

    serialized += "{\n";
    serialized += "  \"total_size\": " + utils::to_string(_total_size) + ",\n";
    serialized +=
        "  \"uploaded_size\": " + utils::to_string(_uploaded_size) + ",\n";
    serialized += "  \"start_time\": " + utils::to_string(_start_time) + ",\n";
    serialized +=
        "  \"upload_time\": " + utils::to_string(upload_time()) + ",\n";
    serialized +=
        "  \"upload_speed\": " + utils::to_string(upload_speed()) + ",\n";
    serialized +=
        "  \"upload_progress\": " + utils::to_string(upload_progress()) + ",\n";
    serialized += "  \"success\": " + utils::to_string(success()) + "\n";
    serialized += "}\n";

    return (serialized);
}

std::string UploadRecord2::_generate_temp_file_path()
{
    std::string folder = "/tmp";
    std::string temp_file_path = folder + webkernel::uuid();

    return (temp_file_path);
}

void UploadRecord2::_open_file_stream(const std::string& file_path)
{
    file_stream.open(file_path.c_str(),
                     std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file_stream.is_open()) {
        throw std::runtime_error("UploadRecord2: failed to open file stream");
    }
}

} // namespace webkernel
