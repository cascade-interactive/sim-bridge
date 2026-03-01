cmake_minimum_required(VERSION 3.20)
project(sim-bridge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/udp_socket.cpp
    src/serial_port.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ws2_32       # Winsock (UDP)
    setupapi     # Serial port discovery
)