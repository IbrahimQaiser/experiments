#include "huffman.hpp"
#include "gtest/gtest.h"
#include <fstream>
#include <sstream>

TEST(HuffmanCodeTest, FromTree)
{
    std::string s = "BBBBAACD";
    std::map<Symbol, uint64_t> freq;
    for (const auto &ch : s)
    {
        ++freq[{static_cast<unsigned char>(ch)}];
    }

    HuffmanTree tree;
    tree.build_from_frequency_table(freq);

    HuffmanCode code;
    code.build_from_tree(tree.get_root());
}

// Tests that after decompression, it is equal to the original byte sequence
void test_compression_and_decompression(const bytes &s)
{
    std::map<Symbol, uint64_t> freq;
    for (const auto &ch : s)
    {
        ++freq[{ch}];
    }

    HuffmanTree tree;
    tree.build_from_frequency_table(freq);

    HuffmanCode code;
    code.build_from_tree(tree.get_root());

    BitStreamWriter writer;
    for (const auto &ch : s)
    {
        code.compress({static_cast<uint8_t>(ch)}, writer);
    }

    auto compressed = writer.as_bytes();

    BitStreamReader reader;
    reader.from_bytes(compressed);
    auto decompressed = code.decompress(reader, s.size());

    ASSERT_EQ(s, decompressed);
}

TEST(HuffmanCodeTest, CompresionAndDecompression)
{
    // Single byte tests
    test_compression_and_decompression({65});
    test_compression_and_decompression({0});
    test_compression_and_decompression({128});
    test_compression_and_decompression({255});

    // Two bytes
    test_compression_and_decompression({0, 3});
    test_compression_and_decompression({3, 0});
    test_compression_and_decompression({1, 1});
    test_compression_and_decompression({0, 0});
    test_compression_and_decompression({65, 65});

    // Three bytes
    test_compression_and_decompression({0, 0, 0});
    test_compression_and_decompression({0, 0, 1});
    test_compression_and_decompression({0, 1, 0});
    test_compression_and_decompression({0, 1, 1});
    test_compression_and_decompression({1, 0, 0});
    test_compression_and_decompression({1, 0, 1});
    test_compression_and_decompression({1, 1, 0});
    test_compression_and_decompression({1, 1, 1});

    // Repeated bytes
    test_compression_and_decompression(bytes(100, 65));
    test_compression_and_decompression(bytes(1000, 1));

    // Random strings
    test_compression_and_decompression({'T', 'h', 'e', ' ', 'q', 'u', 'i', 'c', 'k', ' ', 'b',
                                        'r', 'o', 'w', 'n', ' ', 'f', 'o', 'x', ' ', 'j', 'u',
                                        'm', 'p', 's', ' ', 'o', 'v', 'e', 'r', ' ', 't', 'h',
                                        'e', ' ', 'l', 'a', 'z', 'y', ' ', 'd', 'o', 'g', 's'});

    // All unique bytes
    test_compression_and_decompression({'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'});
    bytes b;
    for (int i = 0; i < 256; i++)
        b.push_back(static_cast<uint8_t>(i));
    test_compression_and_decompression(b);

    for (int i = 0; i < 256; i++)
        b.push_back(static_cast<uint8_t>(i));
    test_compression_and_decompression(b);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}