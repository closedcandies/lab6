#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>

namespace HamArc{
    extern int SECTION_SIZE;// in bytes
    static std::string EXTENTION = ".haf";
    static std::string TEMP_NAME = "tempname";
    static char DELIMITER = ':';

    class Archive;

    struct EncodedFile{
        EncodedFile(const std::string &name, const long long &size, const long long &header_position, const long long &data_position);

        std::string name;
        long long size, header_position, data_position;
    };

    std::vector<EncodedFile> list_files(std::fstream &archive);
    long long add_endata(std::fstream &archive, const std::string &filename, const long long &size);
    std::fstream& remove_enfile(Archive &archive, const EncodedFile& file_info);
    bool fix_line(uint8_t *control_bits, char *line);
    void set_section_size(const int &size);
    void set_temp_name(const std::string &name);
    void set_extention(const std::string &extention);
    void set_delimiter(const char &delimiter);
    bool is_control(long position);


    class Section{
    public:
        Section(char &line, const int& size);
        Section(const int& size);
        ~Section();

        std::vector<char> encode();
        std::vector<char> decode();
        std::vector<char> get_data();
        int size(); // in bytes
        bool empty();

    private:
        char* line;
        int size_;
    };


    class File{
    public:
        std::string name;
        long long size = 0;
        bool exist = false;

        File(const std::string &path);

        std::vector<char> encode();
        bool decode(const Archive &archive);

    private:
        const std::string *path;
        long long position; // current position in file (bytes)
    };


    class Archive{
    public:
        Archive(const std::string& name);

        void rename(const std::string &new_name);

        std::vector<std::string> list_filenames();

        Archive* extract(const std::vector<std::string> &remove_list);
        Archive* add_file(const std::string &file_path);
        Archive* remove_file(const std::string &filename);

        Archive* merge(const Archive &right_archive, const std::string &new_name);

        Section& get_section(const std::string &filename, const long long &start, const int &size) const;

        size_t files_count;
        std::string name;
    };
}
