#include "../include/datapacker.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

std::ostream &print_binary_data(std::ostream &os, std::vector<uint8_t> &bytes)
{
    for (const auto &byt : bytes)
    {
        if (std::isprint(byt))
        {
            os << static_cast<char>(byt);
        }
        else
        {
            os << "\\x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byt);
        }
    }
    os << std::endl;
    return os;
}

int main()
{
    ExperimentData data;
    data.experiment_code = 'u';
    data.location_id = 1376;
    data.timestamp = time(NULL);
    data.experiment_name = "This is a super important experiment!";
    data.samples = {1.15f, -1.32f, 0.1f, 5.614f, 3.1415, 6.623e23, 9e10 - 9, 1.45f, 1.3213e21f};

    std::vector<uint8_t> buffer(sizeof(data.experiment_code) + sizeof(data.location_id) +
                                sizeof(size_t) + data.experiment_name.size() + sizeof(size_t) +
                                data.samples.size() * sizeof(float) + sizeof(data.timestamp));
    uint8_t *buffer_ptr = buffer.data();

    auto n = datapacker::bytes::encode<datapacker::endian::little>(
        buffer_ptr, data.experiment_code, data.location_id, data.timestamp);

    buffer_ptr += n;

    n = datapacker::bytes::encode_length_prefixed<datapacker::endian::little>(buffer_ptr,
                                                                              data.experiment_name);
    buffer_ptr += n;

    n = datapacker::bytes::encode_length_prefixed<datapacker::endian::little>(buffer_ptr,
                                                                              data.samples);

    print_binary_data(std::cout, buffer);

    // Decode the data from the buffer
    ExperimentData data2;
    buffer_ptr = buffer.data();

    n = datapacker::bytes::decode<datapacker::endian::little>(buffer_ptr, data2.experiment_code,
                                                              data2.location_id, data2.timestamp);
    buffer_ptr += n;

    // Limit max number of elements to 1000
    n = datapacker::bytes::decode_length_prefixed<datapacker::endian::little>(
        buffer_ptr, data2.experiment_name, 1000);
    buffer_ptr += n;

    n = datapacker::bytes::decode_length_prefixed<datapacker::endian::little>(buffer_ptr,
                                                                              data2.samples, 1000);

    std::cout << "Original data: " << std::endl;
    data.print();
    std::cout << "=================================================" << std::endl;
    std::cout << "Unpacked data: " << std::endl;
    data2.print();
}
