#include "numbers.hpp"

struct seqData{
    int hits=0, total=0;
};

void update_buffer(double* buffer, int size, int& offset, double n){
    if(offset==size-1){
        for(int i = 0; i < size-1; i++){
            buffer[i]=buffer[i+1];
        }
    }
    else {
        offset++;
    }
    buffer[offset]=n;
}

void rank_values(double* buffer, int* result, int size){
    for(int i = 0; i < size; i++){
        int pos=0;
        for(int o = 0; o < size; o++){
            if(i!=o && buffer[i]>buffer[o]){
                pos++;
            }
        }
        result[i]=pos;
    }
}

int factorial(int n){
    int out=1;
    while(n>0){
        out*=n;
        n--;
    }
    return out;
}

int dataToIndex(int* data, int size){
    int out=0;
    for(int i = 0; i < size; i++){
        int num=data[i];
        for(int o = 0; o < i; o++){
            if(data[i]>=data[o]){
                num--;
            }
        }
        out += factorial(size-i-1)*num;
    }
    return out;
}

void indexToData(int* data, int size, int index){
    bool table[size]; std::memset(table, 1, size);
    for(int i = 0; i < size; i++){
        int scalar = factorial(size-i-1);
        int major = index/scalar;
        index -= major*scalar;
        for(int o = 0; o < size; o++){
            if(table[o]){
                if(major==0){
                    data[i]=o;
                    table[o]=false;
                    break; 
                }
                major--;
            }
        }
    }
}

void crunchNumbers(const std::string& dataFolder, const std::string& outputFileName, int sample_size, int low_times, float low_ratio, uint64_t RAM_GB){
    uint64_t intCount = (RAM_GB*1000000000)/(sizeof(int)*2); uint64_t intRequiredCount=factorial(sample_size);
    /*
    if(intCount>1526751226){
        intCount=1526751226;
    }
    */
    int seqDataChunk=intRequiredCount/intCount;
    if(intRequiredCount%intCount>0){
        seqDataChunk+=1;
    }

    std::cout << "Total processed sequences=" << intRequiredCount << "(int), max data RAM=" << intCount << "(int), num of chunks=" << seqDataChunk << '\n';

    // save all data file names
    std::vector<std::string> data_files;
    for (const auto & file : std::filesystem::directory_iterator(dataFolder)){
        data_files.push_back(file.path());
    }
    
    std::ofstream outputFile(outputFileName);
    seqData *fin_data = new seqData[intCount]; uint64_t ChunkOffset0=0, ChunkOffset1=0;
    for(int i = 0; i < seqDataChunk; i++){
        ChunkOffset1+=intCount;
        if(ChunkOffset1 > intRequiredCount){
            ChunkOffset1=intRequiredCount;
        }
        double price_array[sample_size+1]; int price_array_offset=-1;
        int rank_array[sample_size];
        for(int o = 0; o < data_files.size(); o++){
            // draw percentage line
            if(o%50==0){
                double ratio = (i+((double)o/data_files.size()))/(seqDataChunk)*100;
                std::string sratio = std::to_string(ratio).substr(0, 5);
                std::cout << "[\u001b[50C]" << sratio << '%' << "\u001b[57D";
                int l = 0;
                for(; l < (int)(ratio/2); l++){
                    std::cout << '=';
                }
                std::cout << "\u001b[" << l+1 << "D";
                std::cout.flush();
            }

            // calculate values
            price_array_offset=-1;
            std::ifstream stock_file(data_files[o]); std::string buffer;
            while(std::getline(stock_file, buffer)&&buffer.size()>0){
                update_buffer(price_array, sample_size+1, price_array_offset, std::stod(buffer));
                
                if(price_array_offset==sample_size){
                    rank_values(price_array, rank_array, sample_size);

                    int seqDataIndex = dataToIndex(rank_array, sample_size);
                    if(seqDataIndex>=ChunkOffset0 && seqDataIndex<ChunkOffset1){
                        if(price_array[sample_size] > price_array[sample_size-1]){
                            fin_data[seqDataIndex-ChunkOffset0].hits++;   
                        }
                        fin_data[seqDataIndex-ChunkOffset0].total++;
                    }
                }
            }
            stock_file.close();
        }
        
        // write results to file
        for(int o = 0; o < ChunkOffset1-ChunkOffset0; o++){
            if(fin_data[o].total >= low_times && (float)fin_data[o].hits/fin_data[o].total >= low_ratio){
                indexToData(rank_array, sample_size, ChunkOffset0+o);
                outputFile << rank_array[0];
                for(int p = 1; p < sample_size; p++){
                    outputFile << '|' << rank_array[p];
                }
                outputFile << ' ' << fin_data[o].total << ' ' << fin_data[o].hits << '\n';
            }
            fin_data[o].total=0;
            fin_data[o].hits=0;
        }
        ChunkOffset0=ChunkOffset1;
    }
    std::cout << std::endl;
    outputFile.close();
    delete[] fin_data;
}