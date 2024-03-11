#include "HamArc.h"

namespace HamArc{

    Archive::Archive(const std::string &name) {
        this->name = name;
        files_count = 0;

        //trying to open file with this name
        std::ifstream file;
        file.open(name + EXTENTION, std::ios::binary);
        if (!file){
            std::ofstream new_file;
            new_file.open(name + EXTENTION);
            new_file.close();
        }
    }


    std::vector<std::string> Archive::list_filenames() {
        std::fstream file;
        file.open(name + EXTENTION, std::ios::binary | std::ios::in);
        std::vector<EncodedFile> files = list_files(file);
        for (int i = 0; i < files.size(); i++) {
            std::cout << files[i].name << std::endl;
        }

        std::vector<std::string> filenanames;
        for (const EncodedFile &file : files){
            filenanames.push_back(file.name);
        }
        return filenanames;
    }


    Archive* Archive::extract(const std::vector<std::string> &remove_list) {
        std::fstream archive;
        archive.open(name + EXTENTION, std::ios::in | std::ios::out);
        if (!archive){
            std::cout << "Unable to read archive" << std::endl;
            return this;
        }

        std::vector<EncodedFile> files = list_files(archive);
        for (const std::string &filename : remove_list){    // TODO: maybe print some info if file not found
            for (const EncodedFile &exist : files){
                if (filename == exist.name){
                    File file(filename);
                    file.decode(*this);

                    remove_enfile(*this, exist);
                    break;
                }
            }
        }
        archive.close();
        return this;
    }


    Archive* Archive::add_file(const std::string &file_path) {
        File new_file(file_path);
        if (!new_file.exist){
            std::cout << "File does not exist!" << std::endl;
            return this;
        }
        // open archive file
        std::fstream archive;
        archive.open(this->name + EXTENTION, std::ios::in | std::ios::out | std::ios::binary);
        if (!archive){
            std::cout << "Unable to read archive" << std::endl;
            return this;
        }

        // check if file already exist
        std::vector<EncodedFile> files = list_files(archive);
        for (const EncodedFile &exist : files){
            if (exist.name == new_file.name){
                std::cout << "File already exist" << std::endl;
                return this;
            }
        }

        // insert file
        long long start_pos = add_endata(archive, new_file.name, new_file.size);
        archive.seekp(start_pos, std::ios::beg);

        std::vector<char> encoded_section = new_file.encode();
        while (!encoded_section.empty()){
            for (char symbol : encoded_section){
                archive << symbol;
            }
            encoded_section = new_file.encode();
        }

        files_count++;
        return this;
    }


    Archive* Archive::remove_file(const std::string &filename) {
        std::fstream archive;
        archive.open(name + EXTENTION, std::ios::in | std::ios::binary);
        if (!archive){
            std::cout << "Unable to read archive" << std::endl;
        }
        else {
            std::vector<EncodedFile> files = list_files(archive);
            for (const EncodedFile &file : files){
                if (file.name == filename){
                    remove_enfile(*this, file);
                    break;
                }
            }
        }
        archive.close();
        return this;
    }


    Archive* Archive::merge(const Archive &right_archive, const std::string &new_name) {
        std::fstream new_archive;
        new_archive.open(new_name + TEMP_NAME + EXTENTION, std::ios::out | std::ios::binary);

        // copying data from donor-archives
        std::fstream archive_file;
        for (const Archive &archive : {*this, right_archive}) {
            archive_file.open(archive.name + EXTENTION, std::ios::in | std::ios::binary);

            // copying every file from current donor-archive
            std::vector<EncodedFile> filenames = list_files(archive_file);
            for (const EncodedFile &filename : filenames){
                long long new_position = add_endata(new_archive, filename.name, filename.size);
                long long source_pos = 0;

                new_archive.seekp(new_position, std::ios::beg);
                Section current_section = archive.get_section(filename.name, source_pos, SECTION_SIZE);
                while (!current_section.empty()){
                    for (const char &symbol : current_section.get_data()){
                        new_archive << symbol;
                    }
                    source_pos += current_section.size();
                    current_section = archive.get_section(filename.name, source_pos, SECTION_SIZE);
                }
            }

            std::string temp_name = new_name + TEMP_NAME + EXTENTION, final_name = new_name + EXTENTION;
            std::rename(&temp_name[0], &final_name[0]);
            archive_file.close();
        }


        return this;
    }


    Section& Archive::get_section(const std::string &filename, const long long &start, const int &size) const {
        std::fstream archive;
        archive.open(name + EXTENTION, std::ios::in | std::ios::out);
        if (!archive){
            std::cout << "Unable to read archive" << std::endl;
            Section *result = new Section(0);
            return *result;
        }
        long long start_position, filesize;
        for (const EncodedFile &file : list_files(archive)){
            if (file.name == filename){
                start_position = file.data_position;
                filesize = file.size;
                break;
            }
        }
        long long end_position = start_position + filesize;

        // calculating size of segment
        int segment_size = size;
        if (start_position + size + start > end_position){
            segment_size = end_position - start_position - start;
        }

        if (segment_size == 0){
            Section *result = new Section(0);
            return *result;
        }

        char *buffer = new char[segment_size];
        archive.seekg(start_position - 1 + start, std::ios::beg);
        archive.read(buffer, segment_size);

        archive.close();
        Section *section = new Section(*buffer, segment_size);
        return *section;
    }

    void Archive::rename(const std::string &new_name) {
        std::rename(&this->name[0], &new_name[0]);
    }
}