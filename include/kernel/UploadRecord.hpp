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
    UploadRecord();
    UploadRecord(const std::string& target_filename);
    UploadRecord(const UploadRecord& other);
    UploadRecord& operator=(const UploadRecord& other);
    ~UploadRecord();

  public:
    size_t total_size() const;
    size_t uploaded_size() const;
    float upload_speed() const;
    float upload_progress() const;
    time_t upload_time() const;
    time_t start_time() const;
    std::string target_filename() const;
    std::ofstream& file_stream();

    void update();
    bool success() const;
    std::string serialize() const;

  private:
    size_t _total_size;
    size_t _uploaded_size;
    time_t _start_time;
    std::string _target_filename;
    std::string _temp_filename;
    std::ofstream _file_stream;
    UploadRecordState _state;

  private:
    std::string _generate_temp_file_path(std::string folder);
    void _open_file_stream(const std::string& file_path);
    void _close_file_stream();
};

} // namespace webkernel
