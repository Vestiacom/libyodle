#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "message.hpp"
#include <vector>
#include <iostream>

namespace {

using namespace yodle;

TEST(Message, Construct)
{
    ASSERT_NO_THROW(Message m);
}

TEST(Message, EmptyMessage)
{
    Message m;

    m.kind = 0;
    auto bytes = m.getData();
    for (std::size_t i = 0; i < bytes.size(); ++i) {
        ASSERT_EQ(0, bytes[i]);
    }


    m.kind = 1;
    bytes = m.getData();
    ASSERT_EQ(0x1, bytes[0]);
    for (std::size_t i = 1; i < bytes.size(); ++i) {
        ASSERT_EQ(0, bytes[i]);
    }


    m.kind = 2344;
    bytes = m.getData();
    ASSERT_EQ(0x28, bytes[0]);
    ASSERT_EQ(0x9, bytes[1]);
    for (std::size_t i = 2; i < bytes.size(); ++i) {
        ASSERT_EQ(0, bytes[i]);
    }
}

TEST(Message, NonEmptyMessage)
{
    Message m;

    m.kind = 1;
    m.ss << "12345";

    auto bytes = m.getData();
    // for (std::size_t i = 0; i < bytes.size(); ++i) {
    //     std::cout << std::hex <<  bytes[i];
    // }

    std::size_t i = 0;
    ASSERT_EQ(1, bytes[i++]);
    ASSERT_EQ(0, bytes[i++]);
    ASSERT_EQ(0, bytes[i++]);
    ASSERT_EQ(0, bytes[i++]);
    ASSERT_EQ(5, bytes[i++]);
    ASSERT_EQ(0, bytes[i++]);
    ASSERT_EQ(0, bytes[i++]);
    ASSERT_EQ(0, bytes[i++]);
    ASSERT_EQ('1', bytes[i++]);
    ASSERT_EQ('2', bytes[i++]);
    ASSERT_EQ('3', bytes[i++]);
    ASSERT_EQ('4', bytes[i++]);
    ASSERT_EQ('5', bytes[i++]);
}

} // namespace