#include "block_archiver.hpp"
#include "huffman.hpp"

bytes HuffmanArchiver::compress(bytes::const_iterator beg, const bytes::const_iterator end) const
{
    HuffmanTree tree;

    std::map<Symbol, uint64_t> freq;
    for (auto it = beg; it != end; ++it)
    {
        ++freq[{*it}];
    }
    tree.build_from_frequency_table(freq);

    HuffmanCode code;
    code.build_from_tree(tree.get_root());

    BitStreamWriter writer;
    for (auto it = beg; it != end; ++it)
    {
        code.compress({*it}, writer);
    }
    bytes compressed_data = writer.as_bytes();

    bytes header;
    header.reserve(265);
    // Write the header of the compressed block
    // A plain huffman encoded block has block type 1
    header.push_back(0x1);

    // Write the size of uncompressed data (as little endian)
    bytes temp = to_little_endian(end - beg);
    header.insert(header.end(), temp.begin(), temp.end());
    
    // Write the size of compressed data (as little endian)
    temp = to_little_endian(compressed_data.size());
    header.insert(header.end(), temp.begin(), temp.end());

    // Write the bitstring length table of size 256 bytes
    std::map<Symbol, int> bitstring_length;
    code.compute_bitstring_lengths(tree.get_root(), bitstring_length, 0);

    for (int i = 0; i < 256; i++)
        header.push_back(static_cast<uint8_t>(bitstring_length[{static_cast<uint8_t>(i)}]));

    header.reserve(compressed_data.size());
    header.insert(header.end(), compressed_data.begin(), compressed_data.end());
    return header;
}

bytes HuffmanArchiver::decompress(bytes::const_iterator beg, const bytes::const_iterator end) const
{
    bytes b;
    return b;
}