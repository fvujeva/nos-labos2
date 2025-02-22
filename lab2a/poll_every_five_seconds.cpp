#include <iostream>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <ctime>

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
        int fd = open(device.c_str(), O_WRONLY | O_NONBLOCK);
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
        pfd.events = POLLOUT;
        poll_fds.push_back(pfd);
    }

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    while (true) {
        int ret = poll(poll_fds.data(), poll_fds.size(), 5000);
        if (ret < 0) {
            std::cerr << "poll() error: " << strerror(errno) << std::endl;
            break;
        }

        if (ret == 0) {
            std::cout << "No devices ready for writing within timeout." << std::endl;
            continue;
        }

        std::vector<int> writable_indices;
        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLOUT) {
                writable_indices.push_back(i);
            }
        }

        if (!writable_indices.empty()) {
            int random_index = writable_indices[std::rand() % writable_indices.size()];
            int fd = poll_fds[random_index].fd;

            char random_char = 'A' + (std::rand() % 26); // Random character 'A'-'Z'
            ssize_t bytes_written = write(fd, &random_char, sizeof(random_char));
            if (bytes_written > 0) {
                std::cout << "Sent '" << random_char << "' to fd " << fd << std::endl;
            } else {
                std::cerr << "Write error on fd " << fd << ": " << strerror(errno) << std::endl;
            }
        }
    }

    for (const auto& fd : fds) {
        close(fd);
    }

    return 0;
}
