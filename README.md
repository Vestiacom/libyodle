# Yodle
This is a library for IPC in a C++ application in . It uses wonderful [libev](http://software.schmorp.de/pkg/libev.html).

Yodle provides an implementation of `Channel` that you can use to asynchronously receive and send messages.
You create a `Channel` with libev event loop and a file descriptor. Tipically you'd `socketpair` to obtain two, connected sockets for IPC, but it should also be possible to use pipes. (Sockets are buffered so you'll avoid some potential race conditions after forking)

Then you register some callbacks with `Channel::on`, run `Channel::start` and you're ready to go! It's very simple.

Communication between channels is asynchronous. `Channel::send` puts messages on a queue and whenever underlying fd is ready to be written yodle will serialize the message into a buffer and write it to fd. Receiveing and parsing messages is also asynchronous. Yodle reads and parses the message whenever fd has some data. It handles fragmented messages and messages bigger than the read buffer, so relax. :)

After you're done with the `Channel` you call `Channel::stop` to deregister underlying watchers from libev.

### Message format
In order to pass your data through the `Channel` yodle sends very simple messages. This message will get serialized in one `Channel` and parsed in the other. When all data is received a callback assigned to KIND value is run.


| Name |      Size      |
|------|:--------------:|
| KIND |   sizeof(int)  |
| SIZE | sizeof(size_t) |
| BODY |   SIZE bytes   |


Messages can only be passed with a `std::shared_ptr` to potentially allow passing to another thread or sending one message to multiple channels. 

**Keep in mind to always send messages from the thread that has the libev' event loop** 

# Build
```sh
mkdir build && cd build
cmake ..
make
sudo make install
```

# Test
```sh
./yodle-tests
```

# Example
```cpp

#include <ev++.h>
#include <yodle/yodle.hpp>

enum class KIND {
    A
};

int main() {
    int fd[2];
    ::socketpair(AF_LOCAL, SOCK_STREAM, 0, fd);

    struct ev_loop* loop = EV_DEFAULT;

    int pid = ::fork();
    if (pid == 0) {
        yodle::Channel a(fd[0], loop);

        auto m = yodle::createMessage(KIND::A);
        m->ss << "Body";
        a.send(m);

        a.start();
        ::ev_run(loop, 0);

        ::exit(EXIT_SUCCESS);
    }

    yodle::Channel b(fd[1], loop);
    b.on(KIND::A, [&](const std::shared_ptr<Message> m) {
        std::cout << "Kind: " <<  m->kind << std::endl;
        std::cout << "Size: " <<  m->size << std::endl;
        std::cout << "Body: " <<  m->ss.str() << std::endl;

        b.stop()
        
        ::ev_break(loop, EVBREAK_ALL);
    });

    b.start();

    ::ev_run(loop, 0);

    // Child process won't close itself
    ::kill(pid, SIGKILL);
}
```
