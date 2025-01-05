cmake_minimum_required(VERSION 3.20)
project(PollInputExample)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define the first executable: poll_indef
add_executable(poll_indef poll_indef.cpp)

# Define the second executable: poll_every_five_seconds
add_executable(poll_every_five_seconds poll_every_five_seconds.cpp)
