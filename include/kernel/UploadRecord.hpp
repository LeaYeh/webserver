#include <fstream>
#include <string>
#include <time.h>

namespace webkernel
{

enum UploadRecordState
{
    UPLOAD_ERROR = -1,
    UPLOAD_INIT = 0,
    UPLOADING,
    UPLOAD_COMPLETE,
};

class UploadRecord
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
    std::string target_filename() const;
    bool already_exist() const;

    void update(bool is_last_chunk = false);
    bool success() const;
    std::string serialize() const;

public:
    UploadRecord();
    UploadRecord(const std::string& target_filename, size_t total_size);
    ~UploadRecord();

private:
    size_t _total_size;
    size_t _uploaded_size;
    time_t _start_time;
    std::string _target_filename;
    std::string _temp_filename;
    UploadRecordState _state;
    bool _already_exist;

private:
    std::string _generate_temp_file_path(std::string folder);
    void _open_file_stream(const std::string& file_path);

private:
    UploadRecord(const UploadRecord& other);
    UploadRecord& operator=(UploadRecord other);
};

} // namespace webkernel
