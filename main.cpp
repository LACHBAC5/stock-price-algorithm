#include <iostream>
#include <vector>
#include <python3.10/Python.h>

#include "numbers.hpp"

const char* msg=
"available commands:\n"
"download -> download ticker data\n"
"Provide a list of tickers in a file\n"
"AMZN\\nTSLA\\nGOOGL ...\n"
"Provide a folder to download the data in\n"
"crunch -> analize data\n"
"Provide a folder with data and file for output\n"
"Choose sequence length (a number from 8 to 15)\n"
"Choose min sequence appear rate\n"
"Choose min sequence success rate\n"
"Choose max GB RAM usage\n";

wchar_t* char2wchar_t(const char* buffer) {
    const size_t size = strlen(buffer)+1;
    wchar_t* out = new wchar_t[size];
    mbstowcs(out, buffer, size);
    return out;
}

int main(){
    std::cout << msg;
    Py_Initialize();

    std::string input;
    while(true){
        std::cout << "input& "; std::cin>>input;
        if(input == "quit"){
            break;
        }
        else if(input == "download"){
            std::string file, folder;
            std::cout<<"path to input file: "; std::cin>>file;
            std::cout<<"path to output folder: "; std::cin>>folder;

            wchar_t *args[]={char2wchar_t(file.c_str()), char2wchar_t(folder.c_str())};
            const int argv=sizeof(args)/sizeof(wchar_t*);

            PySys_SetArgv(argv, args);

            for(int i = 0; i < argv; i++){
                delete[] args[i];
            }

            FILE* fp = fopen("load-prices.py", "r");
            PyRun_SimpleFile(fp, "load-prices.py");
        }
        else if(input == "crunch"){
            std::string folder, file;
            int seq_length, seq_appear, GB;
            double seq_success;
            std::cout<<"path to input folder: "; std::cin>>folder;
            std::cout<<"path to output file: "; std::cin>>file;

            std::cout<<"sequence length: "; std::cin>>input;
            seq_length=std::stoi(input);
            std::cout<<"min sequence appear rate: "; std::cin>>input;
            seq_appear=std::stoi(input);
            std::cout<<"min sequence success rate: "; std::cin>>input;
            seq_success=std::stod(input);
            std::cout<<"max usage RAM(GB): "; std::cin>>input;
            GB=std::stoi(input);

            crunchNumbers(folder, file, seq_length, seq_appear, seq_success, GB);
        }
        else
        {
            std::cout << "invalid\n";
        }
    }
    Py_Finalize();
    return 0;
}