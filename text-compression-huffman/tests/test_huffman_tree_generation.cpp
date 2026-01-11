#include "huffman.hpp"
#include "gtest/gtest.h"

TEST(HuffmanCodeTest, Generation_with_no_symbols)
{
    std::map<Symbol, uint64_t> freq;
    HuffmanTree code;
    EXPECT_THROW(code.build_from_frequency_table(freq), std::logic_error);
}

TEST(HuffmanCodeTest, Generation_with_single_symbol)
{
    std::map<Symbol, uint64_t> freq;
    freq[{'a'}] = 3;

    HuffmanTree code;
    code.build_from_frequency_table(freq);
    auto root = code.get_root();
    ASSERT_EQ(root->is_leaf, true);
    ASSERT_EQ(root->left.get(), nullptr);
    ASSERT_EQ(root->right.get(), nullptr);
    ASSERT_EQ(root->value, 3);
    ASSERT_EQ(root->sym.sym, 'a');
}

TEST(HuffmanCodeTest, Generation_with_two_symbols)
{
    std::map<Symbol, uint64_t> freq;
    freq[{'a'}] = 2;
    freq[{'b'}] = 3;

    HuffmanTree code;
    code.build_from_frequency_table(freq);
    auto root = code.get_root();

    ASSERT_EQ(root->is_leaf, false);
    ASSERT_NE(root->left.get(), nullptr);
    ASSERT_NE(root->right.get(), nullptr);
    ASSERT_EQ(root->left->value, 2);
    ASSERT_EQ(root->left->sym.sym, 'a');
    ASSERT_EQ(root->right->value, 3);
    ASSERT_EQ(root->right->sym.sym, 'b');
}

TEST(HuffmanCodeTest, Generation_SampleString)
{
    std::string s = "this is an example of a huffman tree";
    std::map<Symbol, uint64_t> freq;
    for (const auto &ch : s)
    {
        ++freq[{static_cast<unsigned char>(ch)}];
    }

    HuffmanTree code;
    code.build_from_frequency_table(freq);
    auto root = code.get_root();

    ASSERT_EQ(root->value, 36);
    ASSERT_EQ(root->left->value, 16);
    ASSERT_EQ(root->left->left->value, 8);
    ASSERT_EQ(root->left->right->value, 8);

    ASSERT_EQ(root->right->value, 20);
    ASSERT_EQ(root->right->left->value, 8);
    ASSERT_EQ(root->right->right->value, 12);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}