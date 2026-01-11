#include "huffman.hpp"
#include "bitmanip.hpp"
#include <algorithm>
#include <iostream>
using std::make_unique;
using std::unique_ptr;

void HuffmanCode::compute_bitstring_lengths(Node *root, std::map<Symbol, int> &bitstring_length,
                                            int depth)
{
    if (root)
    {
        if (root->is_leaf)
        {
            bitstring_length[root->sym] = depth;
            return;
        }
        compute_bitstring_lengths(root->left.get(), bitstring_length, depth + 1);
        compute_bitstring_lengths(root->right.get(), bitstring_length, depth + 1);
    }
}

void HuffmanTree::build_from_frequency_table(const std::map<Symbol, uint64_t> &frequency)
{
    if (frequency.begin() == frequency.end())
    {
        // The frequency table is empty, throw an exception
        throw std::logic_error("Supplied frequency table is empty");
    }

    std::priority_queue<Node *, std::vector<Node *>, Node_raw_ptr_compartor> nodes;
    // Initially each input symbol will be a leaf node of the Huffman tree
    for (const auto &symbol : frequency)
    {
        nodes.push(new Node(symbol.first, symbol.second));
    }
    // Then pick two smallest nodes by value, combine them into a new node, and add the new node to
    // the priority queue
    while (nodes.size() > 1)
    {
        auto smallest = nodes.top();
        nodes.pop();
        auto second_smallest = nodes.top();
        nodes.pop();

        auto new_node = new Node(smallest->value + second_smallest->value);
        // Transfer ownership of the two smallest nodes to the newly created node
        new_node->left = unique_ptr<Node>(smallest);
        new_node->right = unique_ptr<Node>(second_smallest);

        nodes.push(new_node);
    }

    // The root node is the node left in the priority queue
    root = unique_ptr<Node>(nodes.top());
}

void HuffmanCode::build_from_tree(Node *root)
{
    std::map<Symbol, int> bitstring_length;
    compute_bitstring_lengths(root, bitstring_length, 0);

    std::vector<std::pair<int, Symbol>> symbols;
    for (const auto &pair : bitstring_length)
    {
        // Reverse the order, insert (value, key) pairs into the vector

        // Edge case when there is only a single character
        if (pair.second == 0)
            symbols.push_back(std::make_pair<>(1, pair.first));
        else
            symbols.push_back(std::make_pair<>(pair.second, pair.first));
    }

    std::sort(symbols.begin(), symbols.end());
    build_from_bitstring_length(symbols);
}

void HuffmanCode::build_from_bitstring_length(const std::vector<std::pair<int, Symbol>> &symbols)
{
    // Build the canonical huffman code
    bits code = {0};
    for (int i = 0; i < symbols.size(); i++)
    {
        // Code should be the same length as the bit string length, left pad it with zeros
        bits temp(symbols[i].first - code.size(), 0);
        temp.reserve(symbols[i].first);
        temp.insert(temp.end(), code.begin(), code.end());
        codebook[symbols[i].second] = temp;
        if (i == (symbols.size() - 1))
            break;
        code = left_shift(add(code, 1), symbols[i + 1].first - symbols[i].first);
    }
}

void HuffmanCode::compress(Symbol sym, BitStreamWriter &writer) const
{
    auto iter = codebook.find(sym);
    if (iter == codebook.end())
    {
        throw std::logic_error("Symbol to compress not present in codebook");
    }
    writer.write(iter->second);
}

bytes HuffmanCode::decompress(BitStreamReader &reader, uint64_t num_bytes) const
{
    int bit;
    bytes decompressed;
    bits current;

    // Inefficient solution, implement a better solution later
    std::map<bits, Symbol> reverse_map;
    for (const auto &kv : codebook)
        reverse_map[kv.second] = kv.first;

    while ((bit = reader.read()) != -1)
    {
        current.push_back(bit);
        // Check if this code matches any in the table
        auto iter = reverse_map.find(current);
        if (iter != reverse_map.end())
        {
            decompressed.push_back(iter->second.sym);
            current.clear();
        }

        // If we have processed num_bytes, do not process any more
        if (decompressed.size() == num_bytes)
            break;
    }
    return decompressed;
}