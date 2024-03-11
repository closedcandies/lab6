#include "HamArc.h"

namespace HamArc{

    File::File(const std::string &path) {
        this->path = &path;
        name = "";
        // extracting name of file in case path given
        for (int i = path.size() - 1; i > -1; i--){
            if (path[i] == '/'){
                break;
            }
            name = path[i] + name;
        }

        std::ifstream file;
        file.open(path, std::ios::binary | std::ios::in);
        if (file){
            file.seekg(0, std::ios::end);
            this->size = file.tellg();
            long sections = size / SECTION_SIZE;

            // for one section
            int m = 1;
            while (pow(2, m) - 1 < SECTION_SIZE * 8) {
                m++;
            }
            m = m % 8 == 0 ? m / 8 : m / 8 + 1;

            this->size += m * sections;

            // for last section
            if (size % SECTION_SIZE != 0){
                int add = 1;
                while (pow(2, add) - 1 < size % SECTION_SIZE * 8) {
                    add++;
                }
                this->size += add % 8 == 0 ? add / 8 : add / 8 + 1;
            }

            exist = true;
            position = 0;
        }
    }


    std::vector<char> File::encode() {
        std::vector<char> result;

        //open file
        std::ifstream file;
        file.open(*path, std::ios::binary);
        if (!file){
            std::cout << "File not found" << std::endl;
            return result;
        }

        // get length of file in bytes
        file.seekg(0, std::ios::end);
        long long lengh = file.tellg();
        file.seekg(this->position, std::ios::beg);

        int current_size = SECTION_SIZE;
        if (lengh < this->position + current_size){
            current_size = lengh - this->position;
        }

        if (current_size == 0){
            return result;
        }

        char *buffer = new char[current_size];
        file.read(buffer, current_size);

        Section section = Section(*buffer, current_size);
        result = section.encode();
        this->position += current_size;

        file.close();
        return result;
    }

    bool File::decode(const Archive &archive) {
       // creating a file
       std::ofstream file(this->name);

       int encoded_size = SECTION_SIZE;

        int m = 1;
        while (pow(2, m) - 1 < SECTION_SIZE * 8) {
            m++;
        }
        m = m % 8 == 0 ? m / 8 : m / 8 + 1;

        encoded_size += m;

       long long current_position = 0;
       Section current_section = archive.get_section(this->name, current_position, encoded_size);
       while(!current_section.empty()){
           std::vector<char> result = current_section.decode();
           for (char i : result){
               file << i;
           }
           current_position += current_section.size();
           current_section = archive.get_section(this->name, current_position, encoded_size);
       }

       file.close();
       return true;
    }


}