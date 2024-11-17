#include <string>

class ICodec
{
  public:
    virtual ~ICodec(){};

    virtual std::string decode(const std::string& content) const = 0;
    virtual std::string encode(const std::string& content) const = 0;
    // virtual bool is_valid(const std::string& content) const = 0;
};
