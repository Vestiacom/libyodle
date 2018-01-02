# Yodle
LibYodle is a library for Linux IPC in a C++ application. It uses [libev](http://software.schmorp.de/pkg/libev.html).

# Build
```sh
mkdir build && cd build
cmake ..
make
sudo make install
```

# Test
```sh
./everest-tests --catch_system_errors=no
```
# Message format
| Name |      Size      |
|------|:--------------:|
| KIND |   sizeof(int)  |
| SIZE | sizeof(size_t) |
| BODY |   SIZE bytes   |


# Example usage
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

        ::ev_break(loop, EVBREAK_ALL);
    });

    b.start();

    ::ev_run(loop, 0);

    ::kill(pid, SIGKILL);
}
```
