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
#include <everest/everest.hpp>

enum struct MSG_TYPE { 
	MSG_A	
}
int setup() {
	struct ev_loop* loop = EV_DEFAULT;

	yodle::Channel channel(loop);
	
	// Runs till ev_break is called
	ev_run(loop, 0);
}
```
