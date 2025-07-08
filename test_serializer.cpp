#include "serializer.h"
#include <gtest/gtest.h>
#include <sstream>

// простой POD с serialize-функцией
struct Data {
    uint64_t a;
    bool     b;
    uint64_t c;

    template <class SerializerT>
    Error serialize(SerializerT& s) {
        return s(a, b, c);
    }
};

TEST(SerializerTest, SaveNumbersAndBool) {
    std::stringstream ss;
    Data x{ 42, true, 7 };
    Serializer serializer(ss);
    EXPECT_EQ(serializer.save(x), Error::NoError);
    EXPECT_EQ(ss.str(), "42 true 7");
}

TEST(DeserializerTest, LoadNumbersAndBool) {
    std::stringstream ss("42 true 7");
    Deserializer deserializer(ss);
    Data y{ 0, false, 0 };
    EXPECT_EQ(deserializer.load(y), Error::NoError);
    EXPECT_EQ(y.a, 42ull);
    EXPECT_TRUE(y.b);
    EXPECT_EQ(y.c, 7ull);
}

TEST(DeserializerTest, LoadFalseBool) {
    std::stringstream ss("1 false 2");
    Deserializer d(ss);
    Data y{0,true,0};
    EXPECT_EQ(d.load(y), Error::NoError);
    EXPECT_EQ(y.a, 1ull);
    EXPECT_FALSE(y.b);
    EXPECT_EQ(y.c, 2ull);
}

TEST(DeserializerTest, CorruptedWhenNonDigit) {
    std::stringstream ss("x true 2");
    Deserializer d(ss);
    Data y{0,false,0};
    EXPECT_EQ(d.load(y), Error::CorruptedArchive);
}

TEST(DeserializerTest, CorruptedWhenBadBool) {
    std::stringstream ss("1 yes 2");
    Deserializer d(ss);
    Data y{0,false,0};
    EXPECT_EQ(d.load(y), Error::CorruptedArchive);
}

TEST(DeserializerTest, CorruptedWhenMissingField) {
    std::stringstream ss("1 true");
    Deserializer d(ss);
    Data y{0,false,0};
    EXPECT_EQ(d.load(y), Error::CorruptedArchive);
}

TEST(DeserializerTest, ExtraFieldsAreIgnored) {
    std::stringstream ss("1 false 3 extra data");
    Deserializer d(ss);
    Data y{0,true,0};
    // читаются первые три, лишнее остаётся в потоке
    EXPECT_EQ(d.load(y), Error::NoError);
    EXPECT_EQ(y.a, 1ull);
    EXPECT_FALSE(y.b);
    EXPECT_EQ(y.c, 3ull);
}

/*
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
*/

