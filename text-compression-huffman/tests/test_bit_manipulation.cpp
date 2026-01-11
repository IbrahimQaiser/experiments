#include "bitmanip.hpp"
#include "gtest/gtest.h"

TEST(BitManipulation, BitAdditionSimple)
{
    bits b = {0};
    auto result = add(b, 1);
    ASSERT_EQ(result, (bits{1}));

    result = add(bits{1}, 1);
    ASSERT_EQ(result, (bits{1, 0}));
}

TEST(BitManipulation, BitAddition)
{
    ASSERT_EQ(add(bits{0, 0}, 1), (bits{0, 1}));
    ASSERT_EQ(add(bits{0, 1}, 1), (bits{1, 0}));
    ASSERT_EQ(add(bits{1, 0}, 1), (bits{1, 1}));
    ASSERT_EQ(add(bits{1, 1}, 1), (bits{1, 0, 0}));

    ASSERT_EQ(add(bits{0, 0}, 0), (bits{0, 0}));
    ASSERT_EQ(add(bits{0, 1}, 0), (bits{0, 1}));
    ASSERT_EQ(add(bits{1, 0}, 0), (bits{1, 0}));
    ASSERT_EQ(add(bits{1, 1}, 0), (bits{1, 1}));
}

TEST(BitManipulation, BitAddition3Bits)
{
    ASSERT_EQ(add(bits{0, 0, 0}, 1), (bits{0, 0, 1}));
    ASSERT_EQ(add(bits{0, 0, 1}, 1), (bits{0, 1, 0}));
    ASSERT_EQ(add(bits{0, 1, 0}, 1), (bits{0, 1, 1}));
    ASSERT_EQ(add(bits{0, 1, 1}, 1), (bits{1, 0, 0}));
    ASSERT_EQ(add(bits{1, 0, 0}, 1), (bits{1, 0, 1}));
    ASSERT_EQ(add(bits{1, 0, 1}, 1), (bits{1, 1, 0}));
    ASSERT_EQ(add(bits{1, 1, 0}, 1), (bits{1, 1, 1}));
    ASSERT_EQ(add(bits{1, 1, 1}, 1), (bits{1, 0, 0, 0}));

    ASSERT_EQ(add(bits{0, 0, 0}, 0), (bits{0, 0, 0}));
    ASSERT_EQ(add(bits{0, 0, 1}, 0), (bits{0, 0, 1}));
    ASSERT_EQ(add(bits{0, 1, 0}, 0), (bits{0, 1, 0}));
    ASSERT_EQ(add(bits{0, 1, 1}, 0), (bits{0, 1, 1}));
    ASSERT_EQ(add(bits{1, 0, 0}, 0), (bits{1, 0, 0}));
    ASSERT_EQ(add(bits{1, 0, 1}, 0), (bits{1, 0, 1}));
    ASSERT_EQ(add(bits{1, 1, 0}, 0), (bits{1, 1, 0}));
    ASSERT_EQ(add(bits{1, 1, 1}, 0), (bits{1, 1, 1}));

    ASSERT_EQ(add(bits{1}, 1), (bits{1, 0}));
}

TEST(BitManipulation, LeftShift)
{
    ASSERT_EQ(left_shift(bits{1}, 0), (bits{1}));
    ASSERT_EQ(left_shift(bits{1}, 1), (bits{1, 0}));
    ASSERT_EQ(left_shift(bits{1}, 2), (bits{1, 0, 0}));
    ASSERT_EQ(left_shift(bits{1}, 3), (bits{1, 0, 0, 0}));
    ASSERT_EQ(left_shift(bits{1, 1, 0, 1}, 3), (bits{1, 1, 0, 1, 0, 0, 0}));
}

TEST(BitManipulation, BitStreamWriterEmpty)
{
    BitStreamWriter s;
    auto result = s.as_bytes();
    ASSERT_EQ(result, bytes{});
}

TEST(BitManipulation, BitStreamWriterTests)
{
    BitStreamWriter s;
    s.write(0);
    ASSERT_EQ(s.as_bytes(), bytes{0});

    s.write(0);
    ASSERT_EQ(s.as_bytes(), bytes{0});

    s.write(1);
    ASSERT_EQ(s.as_bytes(), bytes{0x80});

    // Write 7 bytes
    s.write(1);
    s.write(0);
    s.write(1);
    s.write(0);
    s.write(1);
    s.write(0);
    s.write(1);
    ASSERT_EQ(s.as_bytes(), bytes{0xAA});

    // Write 8 bytes
    s.write(1);
    s.write(0);
    s.write(1);
    s.write(1);
    s.write(0);
    s.write(1);
    s.write(0);
    s.write(1);
    ASSERT_EQ(s.as_bytes(), bytes{0xB5});

    s.write(bits{1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0,
                 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1});
    ASSERT_EQ(s.as_bytes(), (bytes{0x95, 0xBA, 0xAE, 0xAA, 0xFB, 0xC1}));

    s.write(bits{1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1,
                 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1});
    ASSERT_EQ(s.as_bytes(), (bytes{0x95, 0xBA, 0xAE, 0xAA, 0xFB, 0xC1, 0x80}));
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}