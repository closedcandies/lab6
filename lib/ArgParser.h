#include <iostream>
#include <string>
#include <vector>
#include <sstream>
namespace ArgumentParser {
    struct Params {
        std::string arh_file_name_ = "";
        std::vector<std::string> files;
        bool create_ = false;
        bool list_ = false;
        bool extract_ = false;
        bool delete_ = false;
        bool append_ = false;
        bool concatenate_ = false;
        uint8_t block_size_ = 10;
    };

    class ArgParser{
    public:
        void Parse(int argc, char** argv);
        void Parse(const std::vector<std::string>& args);

        Params& fGetParams();
    private:
        Params params_;
    };

    bool startsWith(const std::string& str, const std::string& prefix);

    void removeSubstring(std::string& mainStr, const std::string& substr);

}
