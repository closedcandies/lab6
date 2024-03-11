#include <iostream>
#include <fstream>
#include "lib/HamArc.h"
#include "lib/ArgParser.h"


int main(int argc, char *argv[]) {

    ArgumentParser::ArgParser pars;
    pars.Parse(argc, argv);
    ArgumentParser::Params params = pars.fGetParams();

    HamArc::SECTION_SIZE = params.block_size_;
    HamArc::Archive arch(params.arh_file_name_);

    if (params.extract_){
        arch.extract(params.files);
    }
    if (params.list_){
        arch.list_filenames();
    }
    if (params.delete_){
        for (int i = 0; i < params.files.size(); i++){
            arch.remove_file(params.files[i]);
        }
    }
    if (params.append_){
        for (int i = 0; i < params.files.size(); i++){
            arch.add_file(params.files[i]);
        }
    }
    if (params.concatenate_){
        HamArc::Archive arch1(params.files[0]);
        HamArc::Archive arch2(params.files[1]);
        arch1.merge(arch2, arch.name);
    }
    if (params.create_){
        for (int i = 0; i < params.files.size(); i++){
            arch.add_file(params.files[i]);
        }
    }
}



//    ArgumentParser::ArgParser parser("parser");
//
//    parser.AddStringArgument('c', "create", "Create new archive").Positional().MultiValue();
//    parser.AddStringArgument('f', "file", "Archive file name");
//    parser.AddFlag('l', "list", "List archive files");
//    parser.AddStringArgument('x', "extract", "Extract files from archive").Positional().MultiValue();
//    parser.AddStringArgument('d', "delete", "Delete file from archive");
//    parser.AddStringArgument('a', "append", "Append file to archive");
//    parser.AddStringArgument('A', "concatenate", "Merge two archives").Positional().MultiValue();
//
//    parser.Parse(argc, argv);
//
//    if (parser.GetStringUsed("file")) {
//        HamArc::Archive archive(parser.GetStringValue("file"));
//
//        if (parser.GetStringUsed("create")){
//            for (int i = 0; i < parser.GetStringCount("create"); i++){
//                archive.add_file(parser.GetStringValue("create", i));
//            }
//        }
////        if (list.GetValue()){
////            archive.list_filenames();
////        }
////        if (extract.used){
////            for (int i = 0; i < extract.args_count; i++){
////                archive.extract({extract.GetValue(i)});
////            }
////        }
////        if (delete_file.used){
////            archive.remove_file(delete_file.GetValue());
////        }
////        if (append.used){
////            archive.add_file(append.GetValue());
////        }
////        if (concat.used){
////            HamArc::Archive arch1(concat.GetValue(0));
////            HamArc::Archive arch2(concat.GetValue(1));
////            arch1.merge(arch2, archive.name);
////        }
//    }

