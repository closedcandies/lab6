#include "HamArc.h"

namespace HamArc {

    Section::Section(const int &size) {
        size_ = size;
        line = new char[size];
    }

    Section::Section(char &line, const int &size) {
        size_ = size;
        this->line = &line;
    }

    Section::~Section() {
        delete[] line;
    }


    std::vector<char> Section::encode() {
        std::vector<char> result;

        int m = 1;
        while (pow(2, m) - 1 < size_ * 8) {
            m++;
        }

        long encoded_size = size_ * 8 + m;

        int result_size = encoded_size % 8 == 0 ? encoded_size / 8 : encoded_size / 8 + 1;
        for (int i = 0; i < result_size; i++){
            result.push_back(0);
        }

        int current_control_bit = 1, current_data_bit = 0;
        for (int i = 0; i < encoded_size; i++){
            if (i + 1 == current_control_bit){
                current_control_bit *= 2;
                continue;
            }
            else if (line[current_data_bit / 8] & (1 << (7 - (current_data_bit % 8)))){
                result[i / 8] |= 1 << (7 - (i % 8));

                //inverting control bits
                int bit_to_invert = current_control_bit, value_left = i;
                while (bit_to_invert > 0){
                    if (value_left + 1 >= bit_to_invert){
                        result[(bit_to_invert - 1) / 8] ^= 1 << (7 - (bit_to_invert - 1) % 8);
                        value_left -= bit_to_invert;
                    }
                    bit_to_invert /= 2;
                }
                current_data_bit++;
            }
            else{
                current_data_bit++;
            }
        }

        return result;
    }


    std::vector<char> Section::decode() {
        std::vector<char> decoded;

        int control_count = 1;
        while (pow(2, control_count) - 1 < size_ * 8) {
            control_count++;
        }

        int result_size = (size_ * 8 - control_count) / 8;

        for (int i = 0; i < result_size; i++){
            decoded.push_back(0);
        }

        uint8_t control_bits[control_count];
        for (int i = 0; i < control_count; i++){
            control_bits[i] = 0;
        }

        long current_control_bit = 1;
        for (int i = 0; i < size_ * 8; i++){
            if (i + 1 == current_control_bit){
                current_control_bit *= 2;
                continue;
            }
            else if (line[i / 8] & (1 << (7 - (i % 8)))){
                int bit_to_reverse = log2(current_control_bit), data_left = i + 1;
                while (data_left > 0){
                    if (data_left >= 1 << bit_to_reverse){
                        control_bits[bit_to_reverse] ^= 1;
                        data_left -= 1 << bit_to_reverse;
                    }
                    bit_to_reverse--;
                }
            }
        }

        bool readable = fix_line(control_bits, line);

        if (!readable){
            std::cout << "Sorry... This file can be seriously damaged";
        }

        long current_data_bit = 0;
        current_control_bit = 1;
        for (int i = 0; i < size_ * 8; i++){
            if (i + 1 == current_control_bit){
                current_control_bit *= 2;
                continue;
            }
            else if (line[i / 8] & 1 << (7 - (i % 8))){
                decoded[current_data_bit / 8] |= 1 << (7 - (current_data_bit % 8));
                current_data_bit++;
            }
            else{
                current_data_bit++;
            }

            if (current_data_bit == result_size * 8){
                break;
            }
        }

        return decoded;
    }


    std::vector<char> Section::get_data() {
        std::vector<char> result;
        result.reserve(size_);
        for (int i = 0; i < size_; i++){
            result.push_back(line[i]);
        }
        return result;
    }


    int Section::size() {
        return size_;
    }


    bool Section::empty() {
        return size_ == 0;
    }
}