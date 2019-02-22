#include<iostream>
#include<cmath>
#include<fstream>
#include<bitset>
#include<vector>

typedef unsigned int uint32_t;

const int DataPointsRPi=100000;

int main(){
    uint32_t Buffer[DataPointsRPi];
    std::ifstream file("/dev/hsdk", std::ios::binary | std::ios::ate);
    std::streamsize size = DataPointsRPi;
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size))
    {
        for(int i=0;i<DataPointsRPi;i++){

            std::cout<<"\t"<<Buffer[i]<<std::endl;

        }
    }
    else std::cerr<<"Unable to open file"<<std::endl;

    return 0;
}
