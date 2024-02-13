#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>



std::fstream ifs;
std::fstream ofs;
std::string ifile;
std::string ofile;

int init(int argc, char** argv){
    if(argc < 3){
        std::cerr << "needs moar arguments" << std::endl;
        return -1;
    }
    ifile = argv[1];
    std::cout << "in: " <<ifile << std::endl;
    ifs.open(ifile);
    if(!ifs){
        std::cerr << "in::needs openable file" << std::endl;
        return -1;
    }
    ofile = argv[2];
    std::cout << "out: " <<ofile << std::endl;
    ofs.open(ofile, std::fstream::out);
    if(!ofs){
        std::cerr << "out::needs openable file" << std::endl;
        return -1;
    }
    return  0;;
}

struct variable{
    std::string variable_name;
    std::string variable_value;
};



int main(int argc, char** argv){
    
    if(init(argc, argv)) return -1;
    
    ofs << "#include \"any.hpp\"\n\n\n";
    



}