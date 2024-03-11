#include "ArgParser.h"

namespace ArgumentParser {

    Params &ArgParser::fGetParams() {
        return params_;
    }

    bool startsWith(const std::string &str, const std::string &prefix) {
        return str.substr(0, prefix.length()) == prefix;
    }

    void removeSubstring(std::string& mainStr, const std::string& substr) {
        size_t pos = mainStr.find(substr);
        if (pos != std::string::npos) {
            mainStr.erase(pos, substr.length());
        }
    }


    void ArgParser::Parse(const std::vector<std::string> &args) {
        Params &params = fGetParams();
        for (size_t i = 1; i < args.size(); i++) {
            if (args[i] == "-c" || args[i] == "--create") {
                params_.create_ = true;
            } else if (args[i] == "-l" || args[i] == "--list") {
                params_.list_ = true;
            } else if (args[i] == "-x" || args[i] == "--extract") {
                params_.extract_ = true;
                i++;
                while (i < args.size() && !startsWith(args[i], "-")) {
                    params_.files.push_back(args[i]);
                    i++;
                }
                i--;
            } else if (args[i] == "-a" || args[i] == "--append") {
                params_.append_ = true;
                i++;
                while (i < args.size() && !startsWith(args[i], "-")) {
                    params_.files.push_back(args[i]);
                    i++;
                }
                i--;
            } else if (args[i] == "-d" || args[i] == "--delete") {
                params_.delete_ = true;
                i++;
                while (i < args.size() && !startsWith(args[i], "-")) {
                    params_.files.push_back(args[i]);
                    i++;
                }
                i--;
            } else if (args[i] == "-A" || args[i] == "--concatenate") {
                params_.concatenate_ = true;
                i++;
                while (i < args.size() && !startsWith(args[i], "-")) {
                    params_.files.push_back(args[i]);
                    i++;
                }
                i--;
            } else if (startsWith(args[i], "-f") || startsWith(args[i], "--file")) {
                if (startsWith(args[i], "-f")){
                    params.arh_file_name_ = args[i + 1];
                    i++;
                }
                else {
                    params.arh_file_name_ = args[i].substr(args[i].find("=") + 1);
                }
            } else if (startsWith(args[i], "-s") || startsWith(args[i], "--blocksize")) {
                if (startsWith(args[i], "-s")){
                    params.block_size_ = std::stoi(args[i + 1]);
                    i++;
                }
                else {
                    params.block_size_ = std::stoi(args[i].substr(args[i].find("=") + 1));
                }
            }
            else{
                params.files.push_back(args[i]);
            }
        }
    }

    void ArgParser::Parse(int argc, char **argv) {
        std::vector<std::string> args;
        for (int i = 0; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        Parse(args);
    }
}