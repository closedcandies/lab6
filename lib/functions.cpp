#include "HamArc.h"

namespace HamArc {

    int SECTION_SIZE = 10;

    EncodedFile::EncodedFile(const std::string &name, const long long &size, const long long &header_position,
                             const long long &data_position) {
        this->name = name;
        this->size = size;
        this->header_position = header_position;
        this->data_position = data_position;
    }


    std::vector<EncodedFile> list_files(std::fstream &archive) {
        std::vector<EncodedFile> result;

        archive.seekg(0, std::ios::end);
        long long archive_size = archive.tellg();
        if (archive_size == 0){
            return result;
        }

        archive.seekg(0, std::ios::beg);

        char line[1];
        archive.read(line, 1);
        bool flag = true;
        while (flag) {
            if (archive.tellg() >= archive_size) {
                break;
            }

            std::string filename, st_size;
            long long size, header_position, file_position;
            header_position = archive.tellg();
            archive.read(line, 1);
            //extracting filename
            while (line[0] != ':') {
                filename += line[0];
                archive.read(line, 1);
            }

            //extracting size
            archive.read(line, 1);
            while (line[0] != ':') {
                st_size += line[0];
                archive.read(line, 1);
            }
            size = std::stoi(st_size);
            archive.seekg(1, std::ios::cur);
            file_position = archive.tellg();

            archive.seekg(size, std::ios::cur);

            result.push_back(EncodedFile(filename, size, header_position, file_position));
        }

        return result;
    }


    long long add_endata(std::fstream &archive, const std::string &filename, const long long &size) {
        archive.seekg(0, std::ios::end);
        std::string message = DELIMITER + filename + DELIMITER + std::to_string(size) + DELIMITER;
        archive << message;
        return archive.tellg();
    }


    std::fstream& remove_enfile(Archive &archive, const EncodedFile& file_info) {
        std::fstream new_archive, old_archive;
        new_archive.open(TEMP_NAME + EXTENTION, std::ios::out | std::ios::binary);
        old_archive.open(archive.name + EXTENTION, std::ios::in | std::ios::binary);
        if (!new_archive || !old_archive){
            std::cout << "Unable to read archive" << std::endl;
        }

        std::vector<EncodedFile> files = list_files(old_archive);
        for (EncodedFile &file : files){
            if (file.name != file_info.name){
                add_endata(new_archive, file.name, file.size);

                char buffer[file.size];
                old_archive.seekg(file.data_position - 1, std::ios::beg);
                old_archive.read(buffer, file.size);
                new_archive.write(buffer, file.size);
            }
        }

        old_archive.close();
        std::string old_name = TEMP_NAME + EXTENTION, new_name = archive.name + EXTENTION;
        std::rename(&old_name[0], &new_name[0]);

        return new_archive;
    }


    bool fix_line(uint8_t *control_bits, char *line){
        int current_control_bit = 1, error_position = 0;

        int size = 0;
        while (line[size] != '\0'){
            size++;
        }

        while (current_control_bit - 1 < size * 8){
            uint8_t control_bit_fact = line[(current_control_bit - 1) / 8] & 1 << (7 - (current_control_bit - 1) % 8);
            if (control_bit_fact > 0){
                control_bit_fact = 1;
            }
            if (control_bit_fact != control_bits[(int)log2(current_control_bit)]){
                error_position += current_control_bit;
            }
            current_control_bit *= 2;
        }

        if (error_position != 0){
            line[(error_position - 1) / 8] ^= 1 << (7 - (error_position - 1) % 8);
        }
        if (error_position >= size * 8 or is_control(error_position)){
            return false;
        }

        return true;
    }


    void set_section_size(int &size){
        SECTION_SIZE = size;
    }

    void set_temp_name(const std::string &name){
        TEMP_NAME = name;
    }

    void set_extention(const std::string &extention){
        EXTENTION = extention;
    }

    void set_delimiter(const char &delimiter){
        DELIMITER = delimiter;
    }

    bool is_control(long position){
        if (position == 0){
            return false;
        }
        while (position % 2 == 0){
            position /= 2;
        }
        if (position == 1){
            return true;
        }
        else{
            return false;
        }
    }
}
