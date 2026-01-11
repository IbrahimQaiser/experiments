#include "datapacker.h"
#include <gtest/gtest.h>
#include <math.h>
#include <vector>

using namespace datapacker::bytes;
#define encode_length_prefixed datapacker::bytes::encode_length_prefixed<datapacker::endian::big>
#define decode_length_prefixed datapacker::bytes::decode_length_prefixed<datapacker::endian::big>

TEST(EncodingOfIntegers, SingleByte)
{
    uint8_t buffer[16] = {0};
    int8_t a = 50;
    uint8_t b = 60;

    encode_be(buffer, a);
    EXPECT_EQ(buffer[0], 50);
    encode_be(buffer, b);
    EXPECT_EQ(buffer[0], 60);

    encode_le(buffer, a);
    EXPECT_EQ(buffer[0], 50);
    encode_le(buffer, b);
    EXPECT_EQ(buffer[0], 60);

    for (auto i = 1; i < 16; ++i)
        EXPECT_EQ(buffer[i], 0);
}

TEST(EncodingOfIntegers, TwoBytes)
{
    int16_t a = 0x7fac;
    uint16_t b = 0x6e5b;
    uint8_t buffer[16] = {0};

    encode_be(buffer, a);
    EXPECT_EQ(buffer[0], 0x7f);
    EXPECT_EQ(buffer[1], 0xac);
    encode_be(buffer, b);
    EXPECT_EQ(buffer[0], 0x6e);
    EXPECT_EQ(buffer[1], 0x5b);

    encode_le(buffer, a);
    EXPECT_EQ(buffer[0], 0xac);
    EXPECT_EQ(buffer[1], 0x7f);
    encode_le(buffer, b);
    EXPECT_EQ(buffer[0], 0x5b);
    EXPECT_EQ(buffer[1], 0x6e);

    for (auto i = 2; i < 16; ++i)
        EXPECT_EQ(buffer[i], 0);
}

TEST(EncodingOfIntegers, FourBytes)
{
    int32_t a = 0x7fabcdef;
    uint32_t b = 0xf0123456;
    uint8_t buffer[16] = {0};

    encode_be(buffer, a);
    EXPECT_EQ(buffer[0], 0x7f);
    EXPECT_EQ(buffer[1], 0xab);
    EXPECT_EQ(buffer[2], 0xcd);
    EXPECT_EQ(buffer[3], 0xef);
    encode_be(buffer, b);
    EXPECT_EQ(buffer[0], 0xf0);
    EXPECT_EQ(buffer[1], 0x12);
    EXPECT_EQ(buffer[2], 0x34);
    EXPECT_EQ(buffer[3], 0x56);

    encode_le(buffer, a);
    EXPECT_EQ(buffer[0], 0xef);
    EXPECT_EQ(buffer[1], 0xcd);
    EXPECT_EQ(buffer[2], 0xab);
    EXPECT_EQ(buffer[3], 0x7f);
    encode_le(buffer, b);
    EXPECT_EQ(buffer[0], 0x56);
    EXPECT_EQ(buffer[1], 0x34);
    EXPECT_EQ(buffer[2], 0x12);
    EXPECT_EQ(buffer[3], 0xf0);

    for (auto i = 4; i < 16; ++i)
        EXPECT_EQ(buffer[i], 0);
}

TEST(EncodingOfIntegers, EightBytes)
{
    int64_t a = 0x7fabcdef01234567;
    uint64_t b = 0xfedcba9876543210;
    uint8_t buffer[16] = {0};

    encode_be(buffer, a);
    EXPECT_EQ(buffer[0], 0x7f);
    EXPECT_EQ(buffer[1], 0xab);
    EXPECT_EQ(buffer[2], 0xcd);
    EXPECT_EQ(buffer[3], 0xef);
    EXPECT_EQ(buffer[4], 0x01);
    EXPECT_EQ(buffer[5], 0x23);
    EXPECT_EQ(buffer[6], 0x45);
    EXPECT_EQ(buffer[7], 0x67);
    encode_be(buffer, b);
    EXPECT_EQ(buffer[0], 0xfe);
    EXPECT_EQ(buffer[1], 0xdc);
    EXPECT_EQ(buffer[2], 0xba);
    EXPECT_EQ(buffer[3], 0x98);
    EXPECT_EQ(buffer[4], 0x76);
    EXPECT_EQ(buffer[5], 0x54);
    EXPECT_EQ(buffer[6], 0x32);
    EXPECT_EQ(buffer[7], 0x10);

    encode_le(buffer, a);
    EXPECT_EQ(buffer[0], 0x67);
    EXPECT_EQ(buffer[1], 0x45);
    EXPECT_EQ(buffer[2], 0x23);
    EXPECT_EQ(buffer[3], 0x01);
    EXPECT_EQ(buffer[4], 0xef);
    EXPECT_EQ(buffer[5], 0xcd);
    EXPECT_EQ(buffer[6], 0xab);
    EXPECT_EQ(buffer[7], 0x7f);
    encode_le(buffer, b);
    EXPECT_EQ(buffer[0], 0x10);
    EXPECT_EQ(buffer[1], 0x32);
    EXPECT_EQ(buffer[2], 0x54);
    EXPECT_EQ(buffer[3], 0x76);
    EXPECT_EQ(buffer[4], 0x98);
    EXPECT_EQ(buffer[5], 0xba);
    EXPECT_EQ(buffer[6], 0xdc);
    EXPECT_EQ(buffer[7], 0xfe);

    for (auto i = 8; i < 16; ++i)
        EXPECT_EQ(buffer[i], 0);
}

TEST(DecodingOfIntegers, SingleByte)
{
    int8_t a = -72, a1;
    uint8_t b = 50, b1;
    uint8_t buffer[16];

    encode_le(buffer, a);
    decode_le(buffer, a1);

    encode_le(buffer, b);
    decode_le(buffer, b1);

    EXPECT_EQ(a, a1);
    EXPECT_EQ(b, b1);

    for (int8_t i = INT8_MIN; i < INT8_MAX; ++i)
    {
        encode_le(buffer, i);
        decode_le(buffer, a1);
        ASSERT_EQ(i, a1);
    }

    for (uint8_t i = 0; i < UINT8_MAX; ++i)
    {
        encode_le(buffer, i);
        decode_le(buffer, b1);
        ASSERT_EQ(i, b1);
    }

    a = INT8_MAX;
    b = UINT8_MAX;
    encode_le(buffer, a);
    decode_le(buffer, a1);
    encode_le(buffer, b);
    decode_le(buffer, b1);
    ASSERT_EQ(a, a1);
    ASSERT_EQ(b, b1);
}

TEST(DecodingOfIntegers, TwoBytes)
{
    int16_t a = -0x7fac, a1;
    uint16_t b = 0x6e5b, b1;
    uint8_t buffer[16] = {0};

    encode_le(buffer, a);
    decode_le(buffer, a1);
    encode_be(buffer, b);
    decode_be(buffer, b1);

    ASSERT_EQ(a, a1);
    ASSERT_EQ(b, b1);
}

TEST(DecodingOfIntegers, FourBytes)
{
    int32_t a = -0x7fabcdef, a1;
    uint32_t b = 0xf0123456, b1;
    uint8_t buffer[16] = {0};

    encode_le(buffer, a);
    decode_le(buffer, a1);
    encode_be(buffer, b);
    decode_be(buffer, b1);

    ASSERT_EQ(a, a1);
    ASSERT_EQ(b, b1);
}

TEST(DecodingOfIntegers, EightBytes)
{
    int64_t a = -0x7fabcdef01234567, a1;
    uint64_t b = 0xfedcba9876543210, b1;
    uint8_t buffer[16] = {0};

    encode_le(buffer, a);
    decode_le(buffer, a1);
    encode_be(buffer, b);
    decode_be(buffer, b1);

    ASSERT_EQ(a, a1);
    ASSERT_EQ(b, b1);
}

// clang-format off
#define BOUNDARYTEST(dtype, bits)                                \
{                                                                \
    std::vector<dtype> ints;                                     \
    for(int i = 0; i < 32; ++i)                                  \
    {                                                            \
        ints.push_back(static_cast<dtype>(INT##bits##_MIN + i)); \
        ints.push_back(static_cast<dtype>(INT##bits##_MAX - i)); \
        ints.push_back(static_cast<dtype>(i));                   \
    }                                                            \
    constexpr int arr_size = bits / 8;                           \
    uint8_t buffer[arr_size] = {0};                              \
    for (const auto &i : ints) {                                 \
    dtype x;                                                     \
    encode_le(buffer, i);                                        \
    decode_le(buffer, x);                                        \
    ASSERT_EQ(i, x);                                             \
    encode_be(buffer, i);                                        \
    decode_be(buffer, x);                                        \
    ASSERT_EQ(i, x);                                             \
    }                                                            \
    for (int i = 0; i <= 8; i++) {                               \
    u##dtype u = static_cast<u##dtype>(i), u1;                   \
    encode_le(buffer, u);                                        \
    decode_le(buffer, u1);                                       \
    ASSERT_EQ(u, u1);                                            \
    encode_be(buffer, u);                                        \
    decode_be(buffer, u1);                                       \
    ASSERT_EQ(u, u1);                                            \
    u = static_cast<u##dtype>(UINT##bits##_MAX - i);             \
    encode_le(buffer, u);                                        \
    decode_le(buffer, u1);                                       \
    ASSERT_EQ(u, u1);                                            \
    encode_be(buffer, u);                                        \
    decode_be(buffer, u1);                                       \
    ASSERT_EQ(u, u1);                                            \
    }                                                            \
}
// clang-format on

TEST(BoundaryTests, Ints)
{
    BOUNDARYTEST(int8_t, 8);
    BOUNDARYTEST(int16_t, 16);
    BOUNDARYTEST(int32_t, 32);
    BOUNDARYTEST(int64_t, 64);
}

TEST(RealNumberTests, FloatPackingAndUnpacking)
{
    std::vector<float> floats = {FLT_MIN,
                                 FLT_MIN + 1,
                                 FLT_MIN + 2,
                                 -5,
                                 -4,
                                 -3,
                                 -2,
                                 -1,
                                 0,
                                 1,
                                 2,
                                 3,
                                 4,
                                 5,
                                 FLT_MAX - 5,
                                 FLT_MAX - 4,
                                 FLT_MAX - 3,
                                 FLT_MAX - 2,
                                 FLT_MAX - 1,
                                 FLT_MAX,
                                 static_cast<float>(M_PI),
                                 static_cast<float>(M_E),
                                 -static_cast<float>(M_PI),
                                 -static_cast<float>(M_E),
                                 6.62310e23f};
    uint8_t buffer[4];
    float f;

    for (const auto &num : floats)
    {
        encode_float(buffer, num);
        decode_float(buffer, f);
        ASSERT_EQ(f, num);
    }
}

TEST(RealNumberTests, DoublePackingAndUnpacking)
{
    // clang-format off
    std::vector<double> doubles = {DBL_MIN, DBL_MIN + 1, DBL_MIN + 2, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5,
        DBL_MAX - 5, DBL_MAX - 4, DBL_MAX - 3, DBL_MAX - 2, DBL_MAX - 1, DBL_MAX,
        -DBL_MIN, -DBL_MAX,
        0.1, 0.001, -0.001, 1.5, -1.5, 100.0, -100.0, 1e10, -1e10,
        1e-10, -1e-10, 1e-50, -1e-50, 1e300, -1e300, DBL_EPSILON, -DBL_EPSILON,
        M_PI, -M_PI, M_E, -M_E
    };
    // clang-format on
    uint8_t buffer[8];
    double f;

    for (const auto &num : doubles)
    {
        encode_double(buffer, num);
        decode_double(buffer, f);
        ASSERT_EQ(f, num);
    }
}

TEST(MultipleEncodingSameBuffer, Integers)
{
    uint8_t a = 253, a1;
    int8_t b = -126, b1;
    uint16_t c = 65530, c1;
    int16_t d = -32763, d1;
    uint32_t e = 10, e1;
    int32_t f = -572923, f1;
    uint64_t g = 1777777798832, g1;
    int64_t h = -2340978324, h1;
    uint8_t buffer[sizeof(a) + sizeof(b) + sizeof(c) + sizeof(d) + sizeof(e) + sizeof(f) +
                   sizeof(g) + sizeof(h)];

    ASSERT_EQ(encode_le(buffer, a, b, c, d, e, f, g, h), 30);
    ASSERT_EQ(decode_le(buffer, a1, b1, c1, d1, e1, f1, g1, h1), 30);
    ASSERT_EQ(a, a1);
    ASSERT_EQ(b, b1);
    ASSERT_EQ(c, c1);
    ASSERT_EQ(d, d1);
    ASSERT_EQ(e, e1);
    ASSERT_EQ(f, f1);
    ASSERT_EQ(g, g1);
    ASSERT_EQ(h, h1);

    ASSERT_EQ(encode_be(buffer, a, b, c, d, e, f, g, h), 30);
    ASSERT_EQ(decode_be(buffer, a1, b1, c1, d1, e1, f1, g1, h1), 30);
    ASSERT_EQ(a, a1);
    ASSERT_EQ(b, b1);
    ASSERT_EQ(c, c1);
    ASSERT_EQ(d, d1);
    ASSERT_EQ(e, e1);
    ASSERT_EQ(f, f1);
    ASSERT_EQ(g, g1);
    ASSERT_EQ(h, h1);
}

#include <string>

TEST(ArrayEncoding, LengthPrefixedInts)
{
    std::vector<int> a = {3, -1, 2, 0, 6441, INT_MAX, INT_MIN, INT_MAX - 1, INT_MIN + 2};
    std::vector<int> a1(a.size());
    auto bytes_required = a.size() * sizeof(int) + sizeof(std::vector<int>::size_type);
    std::vector<uint8_t> buffer(bytes_required);

    ASSERT_EQ(encode_length_prefixed(buffer.data(), a.data(), a.size()), bytes_required);
    ASSERT_EQ(decode_length_prefixed(buffer.data(), a1.data(), a1.size()), bytes_required);
    ASSERT_EQ(decode_length_prefixed(buffer.data(), a1.data(), 0LL), -1);
    ASSERT_EQ(decode_length_prefixed(buffer.data(), a1.data(), 1LL), -1);
    ASSERT_EQ(decode_length_prefixed(buffer.data(), a1.data(), 4LL), -1);
    ASSERT_EQ(decode_length_prefixed(buffer.data(), a1.data(), 8LL), -1);
    ASSERT_EQ(a, a1);
}

TEST(ArrayEncoding, LengthPrefixedDoubles)
{
    std::vector<double> a = {-1.,       0.,      3.1415,  DBL_MIN, DBL_MAX, -8.383242342,
                             777382e10, 5.64123, 8.83e21, 7e10,    -5e-5,   1e100};
    std::vector<double> a1(a.size());
    auto bytes_required = a.size() * sizeof(double) + sizeof(std::vector<double>::size_type);
    std::vector<uint8_t> buffer(bytes_required);

    ASSERT_EQ(encode_length_prefixed(buffer.data(), a.data(), a.size()), bytes_required);
    ASSERT_EQ(decode_length_prefixed(buffer.data(), a1.data(), a1.size()), bytes_required);
    ASSERT_EQ(decode_length_prefixed(buffer.data(), a1.data(), 0LL), -1);
    ASSERT_EQ(decode_length_prefixed(buffer.data(), a1.data(), 1LL), -1);
    ASSERT_EQ(decode_length_prefixed(buffer.data(), a1.data(), 4LL), -1);
    ASSERT_EQ(decode_length_prefixed(buffer.data(), a1.data(), 8LL), -1);
    ASSERT_EQ(a, a1);
}

TEST(ArrayEncoding, LengthPrefixedChars)
{
    std::vector<char> a = {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
    std::vector<char> a1(a.size());
    auto bytes_required = a.size() * sizeof(char) + sizeof(std::vector<char>::size_type);
    std::vector<uint8_t> buffer(bytes_required);

    ASSERT_EQ(encode_length_prefixed(buffer.data(), a.data(), a.size()), bytes_required);
    ASSERT_EQ(decode_length_prefixed(buffer.data(), a1.data(), a1.size()), bytes_required);
    ASSERT_EQ(decode_length_prefixed(buffer.data(), a1.data(), 1LL), -1);
    ASSERT_EQ(decode_length_prefixed(buffer.data(), a1.data(), 4LL), -1);
    ASSERT_EQ(decode_length_prefixed(buffer.data(), a1.data(), 8LL), -1);
    ASSERT_EQ(a, a1);
}

TEST(StringEncoding, LengthPrefixed)
{
    std::string s = "The quick brown fox jumps over the lazy dogs";
    std::vector<uint8_t> buffer(8 + s.size());
    ASSERT_EQ(encode_length_prefixed(buffer.data(), s), s.size() + sizeof(size_t));
    std::string s2;
    ASSERT_EQ(decode_length_prefixed(buffer.data(), s2, s.size()), s.size() + sizeof(size_t));
    ASSERT_EQ(s, s2);

    s2.clear();
    ASSERT_EQ(decode_length_prefixed(buffer.data(), s2, s.size() * 2), s.size() + sizeof(size_t));
    ASSERT_EQ(s, s2);

    s2.clear();
    ASSERT_EQ(decode_length_prefixed(buffer.data(), s2, s.size() - 5), -1);

    s2.clear();
    s = "a";
    ASSERT_EQ(encode_length_prefixed(buffer.data(), s), 1 + sizeof(size_t));
    ASSERT_EQ(decode_length_prefixed(buffer.data(), s2, 1), 1 + sizeof(size_t));
    ASSERT_EQ(s, s2);
}

TEST(VectorEncoding, LengthPrefixed)
{
    std::vector<float> f = {1.1f, -1.3f, 1e10f, FLT_MIN, FLT_MAX, 0.0013f, 5e-5f};
    std::vector<uint8_t> buffer(8 + f.size() * sizeof(float));
    ASSERT_EQ(encode_length_prefixed(buffer.data(), f), f.size() * sizeof(float) + sizeof(size_t));
    std::vector<float> f2;
    ASSERT_EQ(decode_length_prefixed(buffer.data(), f2, f.size()),
              f.size() * sizeof(float) + sizeof(size_t));
    ASSERT_EQ(f, f2);

    f2.clear();
    ASSERT_EQ(decode_length_prefixed(buffer.data(), f2, f.size() * 2),
              f.size() * sizeof(float) + sizeof(size_t));
    ASSERT_EQ(f, f2);

    f2.clear();
    ASSERT_EQ(decode_length_prefixed(buffer.data(), f2, f.size() - 5), -1);

    f = {1.13f};
    ASSERT_EQ(encode_length_prefixed(buffer.data(), f), sizeof(float) + sizeof(size_t));
    ASSERT_EQ(decode_length_prefixed(buffer.data(), f2, 1), sizeof(float) + sizeof(size_t));
    ASSERT_EQ(f, f2);

    f.clear();
    ASSERT_EQ(encode_length_prefixed(buffer.data(), f), sizeof(size_t));
    ASSERT_EQ(decode_length_prefixed(buffer.data(), f2, 0), sizeof(size_t));
    ASSERT_EQ(f, f2);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}