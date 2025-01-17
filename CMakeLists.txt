cmake_minimum_required(VERSION 3.10)
project(UrlShortener)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIRS})

# Добавление пути к заголовочным файлам
include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(jsoncpp REQUIRED)
include_directories(${JSONCPP_INCLUDE_DIRS})

# исходники
add_executable(url_shortener src/main.cpp src/Server.cpp src/Storage.cpp)

# Линковка с Boost и JsonCpp
target_link_libraries(url_shortener Boost::system Boost::thread jsoncpp_lib)

