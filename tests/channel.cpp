#include "gtest/gtest.h"
#include <memory>
#include "channel.hpp"
#include <sys/socket.h>
#include <signal.h>
#include <algorithm>
namespace {

using namespace yodle;

const int TEST_MESSAGE_KIND = 14;
const std::string TEST_MESSAGE_BODY = "THIS IS A TEST MESSAGE BODY";

TEST(Channel, Bad)
{
    ASSERT_ANY_THROW(Channel ch(-1, EV_DEFAULT));
}

TEST(Channel, Construct)
{
    int fd[2];
    ASSERT_EQ(0, socketpair(AF_LOCAL, SOCK_STREAM, 0, fd));

    ASSERT_NO_THROW(Channel ch1(fd[0], EV_DEFAULT));
    ASSERT_NO_THROW(Channel ch2(fd[1], EV_DEFAULT));
}

TEST(Channel, RegisterHandler)
{
    int fd[2];
    ASSERT_EQ(0, socketpair(AF_LOCAL, SOCK_STREAM, 0, fd));

    Channel ch(fd[0], EV_DEFAULT);
    ch.on(1, [&](const std::shared_ptr<Message> m) {
        (void) m;
    });

    ch.on(2, [&](const std::shared_ptr<Message> m) {
        (void) m;
    });

    ch.on(2, [&](const std::shared_ptr<Message> m) {
        (void) m;
    });

}

TEST(Channel, EmptyBody)
{
    int fd[2];
    ASSERT_EQ(0, socketpair(AF_LOCAL, SOCK_STREAM, 0, fd));

    struct ev_loop* loop = EV_DEFAULT;

    int pid = fork();
    ASSERT_NE(-1, pid);

    if (pid == 0) {
        Channel a(fd[0], loop);

        auto m = std::make_shared<Message>();
        m->kind = TEST_MESSAGE_KIND;
        a.send(m);

        a.start();
        ev_run(loop, 0);

        exit(EXIT_SUCCESS);
    }

    Channel b(fd[1], loop);
    b.on(TEST_MESSAGE_KIND, [&](const std::shared_ptr<Message> m) {
        EXPECT_EQ(TEST_MESSAGE_KIND, m->kind);
        ev_break(loop, EVBREAK_ALL);
    });

    b.start();

    ev_run(loop, 0);

    ::kill(pid, SIGKILL);
}

TEST(Channel, WithBody)
{
    int fd[2];
    ASSERT_EQ(0, socketpair(AF_LOCAL, SOCK_STREAM, 0, fd));

    struct ev_loop* loop = EV_DEFAULT;

    int pid = fork();
    ASSERT_NE(-1, pid);

    if (pid == 0) {
        Channel a(fd[0], loop);

        auto m = std::make_shared<Message>();
        m->kind = TEST_MESSAGE_KIND;
        m->ss << TEST_MESSAGE_BODY;
        a.send(m);

        a.start();
        ev_run(loop, 0);

        exit(EXIT_SUCCESS);
    }

    Channel b(fd[1], loop);
    b.on(TEST_MESSAGE_KIND, [&](const std::shared_ptr<Message> m) {
        EXPECT_EQ(TEST_MESSAGE_KIND, m->kind);
        EXPECT_EQ(TEST_MESSAGE_BODY, m->ss.str());
        ev_break(loop, EVBREAK_ALL);
    });

    b.start();

    ev_run(loop, 0);

    ::kill(pid, SIGKILL);
}

std::string random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

TEST(Channel, BodyBiggerThanBuffer)
{
    std::string body = random_string(4096*100);

    int fd[2];
    ASSERT_EQ(0, socketpair(AF_LOCAL, SOCK_STREAM, 0, fd));

    struct ev_loop* loop = EV_DEFAULT;

    int pid = fork();
    ASSERT_NE(-1, pid);

    if (pid == 0) {
        Channel a(fd[0], loop);

        auto m = std::make_shared<Message>();
        m->kind = TEST_MESSAGE_KIND;
        m->ss << body;
        a.send(m);

        a.start();
        ev_run(loop, 0);

        exit(EXIT_SUCCESS);
    }

    Channel b(fd[1], loop);
    b.on(TEST_MESSAGE_KIND, [&](const std::shared_ptr<Message> m) {
        EXPECT_EQ(TEST_MESSAGE_KIND, m->kind);
        EXPECT_EQ(body, m->ss.str());
        ev_break(loop, EVBREAK_ALL);
    });

    b.start();

    ev_run(loop, 0);

    ::kill(pid, SIGKILL);
}



TEST(Channel, Load)
{
    int counter = 0;

    int fd[2];
    ASSERT_EQ(0, socketpair(AF_LOCAL, SOCK_STREAM, 0, fd));

    struct ev_loop* loop = EV_DEFAULT;

    int pid = fork();
    ASSERT_NE(-1, pid);

    if (pid == 0) {
        Channel a(fd[0], loop);
        a.on(TEST_MESSAGE_KIND, [&](const std::shared_ptr<Message> m) {
            a.send(m);
        });

        auto m = std::make_shared<Message>();
        m->kind = TEST_MESSAGE_KIND;
        m->ss << TEST_MESSAGE_BODY;
        a.send(m);

        a.start();
        ev_run(loop, 0);

        exit(EXIT_SUCCESS);
    }

    Channel b(fd[1], loop);
    b.on(TEST_MESSAGE_KIND, [&](const std::shared_ptr<Message> m) {
        EXPECT_EQ(TEST_MESSAGE_KIND, m->kind);
        EXPECT_EQ(TEST_MESSAGE_BODY, m->ss.str());

        counter++;
        if (counter > 100) {
            ev_break(loop, EVBREAK_ALL);
            return;
        }

        b.send(m);
    });

    b.start();

    ev_run(loop, 0);

    ::kill(pid, SIGKILL);
}

} // namespace