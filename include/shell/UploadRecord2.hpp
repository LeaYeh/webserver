#include <fstream>
#include <string>
#include <time.h>

#pragma once

namespace webshell
{

enum UploadRecordState2
{
    UPLOAD_ERROR = -1,
    UPLOAD_INIT = 0,
    UPLOADING,
    UPLOAD_COMPLETE,
};

class UploadRecord2
{
public:
    std::ofstream file_stream;

public:
    size_t total_size() const;
    size_t uploaded_size() const;
    float upload_speed() const;
    float upload_progress() const;
    time_t upload_time() const;
    time_t start_time() const;
    std::string temp_filename() const;

    void update(bool is_last_chunk = false);
    bool success() const;
    std::string serialize() const;

public:
    UploadRecord2();
    UploadRecord2(size_t total_size);
    ~UploadRecord2();

private:
    size_t _total_size;
    size_t _uploaded_size;
    time_t _start_time;
    std::string _temp_filename;
    UploadRecordState2 _state;

private:
    std::string _generate_temp_file_path();
    void _open_file_stream(const std::string& file_path);

private:
    UploadRecord2(const UploadRecord2& other);
    UploadRecord2& operator=(UploadRecord2 other);
};

} // namespace webkernel
