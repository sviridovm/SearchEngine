cmake_minimum_required(VERSION 3.28)

PROJECT(SearchEngine)

file(GLOB_RECURSE SOURCES "src/*.cpp")

file(GLOB_RECURSE HEADERS "include/*.hpp" "include/*.h")

file(GLOB_RECURSE PROTO_HEADERS "proto/*.h")
file(GLOB_RECURSE PROTO_SRCS "proto/*.cc")




# use c++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_library(frontier_proto
    proto/frontier.pb.cc
    proto/frontier.grpc.pb.cc
)

target_include_directories(frontier_proto PUBLIC
    proto
)

find_package(gRPC REQUIRED)
find_package(Protobuf REQUIRED)

target_link_libraries(frontier_proto PUBLIC
    gRPC::grpc++
    protobuf::libprotobuf
)


add_executable(frontier_server 
    app/frontier_server.cpp
    ${SOURCES}
    ${HEADERS}
)

# add_executable(crawler
#     app/crawler.cpp
#     "${SOURCES}"
# )

# add_executable(search_engine
#     app/search_engine.cpp
#     "${SOURCES}"
# )

find_package(OpenSSL REQUIRED)



target_link_libraries(frontier_server PRIVATE 
    OpenSSL::SSL
    frontier_proto
)

target_include_directories(frontier_server PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/include
)



