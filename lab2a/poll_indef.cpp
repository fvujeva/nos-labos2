#include <iostream>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <cstring>
#include <cerrno>

int main() {
    std::vector<std::string> devices = {
            "/dev/shofer0",
            "/dev/shofer1",
            "/dev/shofer2",
            "/dev/shofer3",
            "/dev/shofer4",
            "/dev/shofer5"
    };

    std::vector<int> fds;

    for (const auto& device : devices) {
        int fd = open(device.c_str(), O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
            std::cerr << "Error opening " << device << ": " << strerror(errno) << std::endl;
            continue;
        }
        fds.push_back(fd);
        std::cout << "Opened " << device << " successfully." << std::endl;
    }

    if (fds.empty()) {
        std::cerr << "No devices could be opened. Exiting." << std::endl;
        return 1;
    }

    std::vector<pollfd> poll_fds;
    for (const auto& fd : fds) {
        pollfd pfd;
        pfd.fd = fd;
        pfd.events = POLLIN;
        poll_fds.push_back(pfd);
    }

    std::cout << "Waiting for input..." << std::endl;

    while (true) {
        int ret = poll(poll_fds.data(), poll_fds.size(), -1);
        if (ret < 0) {
            std::cerr << "poll() error: " << strerror(errno) << std::endl;
            break;
        }

        for (const auto& pfd : poll_fds) {
            if (pfd.revents & POLLIN) {
                char buffer[1];
                ssize_t bytesRead = read(pfd.fd, buffer, sizeof(buffer));
                if (bytesRead > 0) {
                    std::cout << "Read from fd " << pfd.fd << ": " << buffer[0] << std::endl;
                } else if (bytesRead == 0) {
                    std::cout << "EOF on fd " << pfd.fd << std::endl;
                } else {
                    std::cerr << "Read error on fd " << pfd.fd << ": " << strerror(errno) << std::endl;
                }
            }
        }
    }

    for (const auto& fd : fds) {
        close(fd);
    }

    return 0;
}

