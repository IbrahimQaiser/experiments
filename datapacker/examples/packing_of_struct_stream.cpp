#include "../include/datapacker.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace datapacker;
using namespace datapacker::stream;

// A sample struct, to demonstrate packing and unpacking
struct ExperimentData
{
    char experiment_code;
    int location_id;
    long long int timestamp;
    std::vector<float> samples;
    std::string experiment_name;

    void print() const
    {
        std::cout << std::dec;
        std::cout << "Experiment Code: " << experiment_code << std::endl;
        std::cout << "Location ID: " << location_id << std::endl;
        std::cout << "Timestamp: " << timestamp << std::endl;
        std::cout << "Experiment Name: " << experiment_name << std::endl;
        std::cout << "Samples: ";
        for (const auto &sample : samples)
        {
            std::cout << sample << " ";
        }
        std::cout << std::endl;
    }
};
int main()
{

    ExperimentData data, data2;
    data.experiment_code = 'u';
    data.location_id = 1376;
    data.timestamp = time(NULL);
    data.experiment_name = "This is a super important experiment!";
    data.samples = {1.15f, -1.32f, 0.1f, 5.614f, 3.1415, 6.623e23, 9e10 - 9, 1.45f, 1.3213e21f};

    std::ostringstream oss;
    write<endian::little>(oss, data.experiment_code);
    write<endian::little>(oss, data.location_id);
    write<endian::little>(oss, data.timestamp);
    write<endian::little>(oss, data.experiment_name);
    write<endian::little>(oss, data.samples);

    std::string str = oss.str();
    std::istringstream iss(str);
    
    std::cout << "=================================================" << std::endl;
    read<endian::little>(iss, data2.experiment_code);
    read<endian::little>(iss, data2.location_id);
    read<endian::little>(iss, data2.timestamp);
    read<endian::little>(iss, data2.experiment_name);
    read<endian::little>(iss, data2.samples);

    std::cout << "Original data: " << std::endl;
    data.print();
    std::cout << "=================================================" << std::endl;
    std::cout << "Unpacked data: " << std::endl;
    data2.print();
}
