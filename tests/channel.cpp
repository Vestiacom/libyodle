#include "gtest/gtest.h"
#include "channel.hpp"

namespace {
using namespace yodle;

TEST(Channel, Bad)
{
    ASSERT_ANY_THROW(Channel ch(-1, EV_DEFAULT));
}

TEST(Channel, Construct)
{
    int fd[2];
    ASSERT_EQ(pipe(fd), 0);

    ASSERT_NO_THROW(Channel ch1(fd[0], EV_DEFAULT));
    ASSERT_NO_THROW(Channel ch2(fd[1], EV_DEFAULT));
}

} // namespace