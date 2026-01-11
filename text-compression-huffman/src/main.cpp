#include "block_archiver.hpp"
#include "file_format.hpp"
#include "huffman.hpp"
#include <algorithm>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <string.h>

// Buffer size 64KB
#define BUFFER_SIZE 64000

int main(int argc, char *argv[])
{
    HuffmanArchiver huff;
    BlockArchiver &archiver = huff;
    bool is_current_mode_compression = true;
    FileFormatHeaderV1 headerv1;
    FileFormatHeader &header = headerv1;

    if (argc < 4)
    {
        std::cerr << "Usage: ./archiver compress|decompress <input_file> <output_file>"
                  << std::endl;
        exit(1);
    }

    if (std::string(argv[1]) == "compress")
        is_current_mode_compression = true;
    else if (std::string(argv[1]) == "decompress")
        is_current_mode_compression = false;
    else
    {
        std::cerr << "Invalid operation, specify either compress or decompress" << std::endl;
        exit(1);
    }

    std::ifstream ifs(argv[2], std::ifstream::binary);
    if (!ifs)
    {
        std::cerr << "Error while opening input file: " << strerror(errno) << std::endl;
        exit(1);
    }

    std::ofstream ofs(argv[3], std::ofstream::binary);
    if (!ifs)
    {
        std::cerr << "Error while opening output file: " << strerror(errno) << std::endl;
        exit(1);
    }

    bytes buffer(BUFFER_SIZE, 0);
    // If compression, write the file header

    if (is_current_mode_compression)
    {
        auto header_buffer = header.create_header();
        ofs.write(reinterpret_cast<char *>(header_buffer.data()), header_buffer.size());

        while (ifs.read(reinterpret_cast<char *>(buffer.data()), buffer.size()) || ifs.gcount() > 0)
        {
            uint64_t bytes_read = ifs.gcount();
            bytes processed;
            processed = archiver.compress(buffer.begin(), buffer.begin() + bytes_read);
            ofs.write(reinterpret_cast<char *>(processed.data()), processed.size());
            if (ofs.bad())
            {
                std::cerr << "Error while writing to output file: " << strerror(errno) << std::endl;
                exit(1);
            }
        }
        ofs.close();
    }
    else
    {
        // If decompression, read the file header and verify if it is a file compressed by this
        // program
        if (ifs.read(reinterpret_cast<char *>(buffer.data()), header.size()) || ifs.gcount() > 0)
        {
            if (!header.verify_header(buffer.begin(), buffer.begin() + header.size()))
            {
                std::cerr << "File is not a valid archive, cannot decompress" << std::endl;
                exit(1);
            }
        }

        uint8_t block_type;
        while (ifs.read(reinterpret_cast<char *>(&block_type), sizeof(block_type)) ||
               ifs.gcount() > 0)
        {
            // TODO: Implement this as an array of archivers, but, it is fine with just one archiver

            if (block_type != 1)
            {
                std::cerr << "Invalid block type detected in file, cannot decompress" << std::endl;
                ofs.close();
                exit(1);
            }

            // Read the uncompressed size of the block
            bytes size_buffer(8, 0);
            ifs.read(reinterpret_cast<char *>(size_buffer.data()), 8);
            if (ifs.gcount() != 8)
            {
                std::cerr << "Error while reading input file: " << std::endl;
                ofs.close();
                exit(1);
            }

            auto uncompressed_size = from_little_endian_to_uint64_t(size_buffer);

            // Read the compressed size of the block
            ifs.read(reinterpret_cast<char *>(size_buffer.data()), 8);
            if (ifs.gcount() != 8)
            {
                std::cerr << "Error while reading input file: " << std::endl;
                ofs.close();
                exit(1);
            }

            auto compressed_size = from_little_endian_to_uint64_t(size_buffer);

            // Read the frequency table
            uint8_t frequency_table[256];
            ifs.read(reinterpret_cast<char *>(frequency_table), 256);
            if (ifs.gcount() != 256)
            {
                std::cerr << "Error while reading input file: " << std::endl;
                ofs.close();
                exit(1);
            }

            std::vector<std::pair<int, Symbol>> bitstring_length;
            for (int i = 0; i < 256; i++)
            {
                if (frequency_table[i] != 0)
                {
                    bitstring_length.push_back(
                        std::make_pair<int, Symbol>(static_cast<int>(frequency_table[i]),
                                                    Symbol{static_cast<unsigned char>(i)}));
                }
            }
            std::sort(bitstring_length.begin(), bitstring_length.end());

            // Build the huffman code
            HuffmanCode code;
            code.build_from_bitstring_length(bitstring_length);

            // std::cout << uncompressed_size << std::endl;
            // std::cout << compressed_size << std::endl;

            // Read the compressed block
            buffer.resize(compressed_size);
            ifs.read(reinterpret_cast<char *>(buffer.data()), compressed_size);
            if (ifs.gcount() != compressed_size)
            {
                std::cerr << "Error while reading input file: " << std::endl;
                ofs.close();
                exit(1);
            }

            BitStreamReader reader;
            reader.from_bytes(buffer);
            auto processed = code.decompress(reader, uncompressed_size);

            ofs.write(reinterpret_cast<char *>(processed.data()), processed.size());
            if (ofs.bad())
            {
                std::cerr << "Error while writing to output file: " << strerror(errno) << std::endl;
                exit(1);
            }
        }

        ofs.close();
    }

    // Check if an error occured
    if (ifs.bad())
    {
        std::cerr << "Error while reading input file: " << strerror(errno) << std::endl;
        exit(1);
    }
    if (ofs.bad())
    {
        std::cerr << "Error while writing to output file: " << strerror(errno) << std::endl;
        exit(1);
    }
}