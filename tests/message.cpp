#include "gtest/gtest.h"
#include "message.hpp"
#include <vector>

namespace {

using namespace yodle;

TEST(Message, Construct)
{
    ASSERT_NO_THROW(Message m);
}

TEST(Message, GetData)
{
    Message m;
    m.kind = 0;
    m.size = 0;

    std::vector<char> e;
    ASSERT_TRUE(m.getData() == e);
    // ASSERT_TRUE(false) << m.getData;
}

} // namespace