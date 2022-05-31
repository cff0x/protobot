#
# ProtoBot - modular, dynamic and extensible Discord bot
# Author: codefox
# License: MIT
#
# Requires: D++ (DPP), CLI11, PostgreSQL (PQXX), OpenSSL and pthreads
#

cmake_minimum_required(VERSION 3.18.4)

# use C++23
set(CMAKE_CXX_STANDARD 20)

# linker flags to use non-apple c++ standard libraries
if(APPLE)
    set(CMAKE_MODULE_LINKER_FLAGS
        "${CMAKE_MODULE_LINKER_FLAGS} -L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib")
endif()

#set(POSTGRESQL_INCLUDE_DIR "C:\\Program Files\\libpqxx\\master_x64\\include")
#set(POSTGRESQL_LIBRARY "C:\\Program Files\\libpqxx\\master_x64\\lib\\Debug\\pqxx.lib")
#set(POSTGRESQL_LIBRARY_DIR "C:\\Program Files\\libpqxx\\master_x64\\lib\\Debug")
#set(PQ_LIBRARY "C:\\Program Files\\libpqxx\\master_x64\\bin\\libpq.lib")
#set(SPDLOG_INCLUDE_DIR "F:\\Development\\spdlog\\include")
#set(SPDLOG_LIBRARY "F:\\Development\\spdlog\\build\\Debug\\spdlogd.lib")
#set(SPDLOG_LIBRARY_DIR "F:\\Development\\spdlog\\build\\Debug")
#set(DPP_INCLUDE_DIR "C:\\Program Files\\libdpp-10.0.9-win64\\include\\dpp-10.0")
#set(DPP_LIBRARY "C:\\Program Files\\libdpp-10.0.9-win64\\lib\\dpp-10.0\\dpp.lib")
#set(DPP_LIBRARY_DIR "C:\\Program Files\\libdpp-10.0.9-win64\\lib\\dpp-10.0")
#set(WINDOWS_EXPORT_ALL_SYMBOLS ON)
#option(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" ON)

#### base project settings
# ProtoBot base CMake project file
project("protobot")

# executable + shared library names
set(PROJECT_EXECUTABLE_NAME "${PROJECT_NAME}")
set(PROJECT_LIBRARY_NAME "lib${PROJECT_NAME}")


######## third-party libraries
# CMake modules for find_package
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

# third-party library folder
set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/thirdparty)


#### CLI11
# set package path
set(CLI11_PATH ${THIRDPARTY_DIR}/CLI11)
set(CLI11_INCLUDE_DIR ${CLI11_PATH}/include)

# add to project
add_subdirectory(${CLI11_PATH})

# add directories to project
include_directories(${CLI11_INCLUDE_DIR})

find_package(Sqlpp11 REQUIRED COMPONENTS PostgreSQL)


#### OpenSSL
# search package
if(APPLE)
    if(CMAKE_APPLE_SILICON_PROCESSOR)
        set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl")
    else()
        set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
    endif()
    find_package(OpenSSL REQUIRED)
else()
    find_package(OpenSSL REQUIRED)
endif()

# add include directory
include_directories(${OPENSSL_INCLUDE_DIR})


#### pthreads library
# prefer pthreads
#set(THREADS_PREFER_PTHREAD_FLAG TRUE)

# search package
set(CMAKE_THREAD_PREFER_PTHREAD FALSE)
set(THREADS_PREFER_PTHREAD_FLAG FALSE)
find_package(Threads REQUIRED)


#### SpdLog
# search package
#find_package(SpdLog REQUIRED)

# add directories to project
include_directories(${SPDLOG_INCLUDE_DIR})
link_directories(${SPDLOG_LIBRARY_DIR})


#### D++ Discord API Library
# search for DPP library
find_package(DPP REQUIRED)

# add directories to project
include_directories(${DPP_INCLUDE_DIR})
link_directories(${DPP_LIBRARY_DIR})

add_custom_target(generate_events
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/build-tools/event_generator.py ${DPP_INCLUDE_DIR}/cluster.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


######## ProtoBot
#### set main source and include directories
aux_source_directory("libprotobot" LIBRARY_SRC_ROOT)
set(PROTOBOT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libprotobot/include)
include_directories(${PROTOBOT_INCLUDE_DIR})

#### build ProtoBot shared library
# add ProtoBot library target
add_library(
    ${PROJECT_LIBRARY_NAME}
    SHARED
    ${LIBRARY_SRC_ROOT}
)

# link ProtoBot library with third-party libraries
target_link_libraries(
    ${PROJECT_LIBRARY_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENSSL_CRYPTO_LIBRARY}
    ${OPENSSL_SSL_LIBRARY}
        ${SPDLOG_LIBRARY}
    ${DPP_LIBRARY}
        pq
        pqxx
        sqlpp11::sqlpp11
        sqlpp11::postgresql
)


# remove prefix from ProtoBot library file name (otherwise "lib" would be prefixed twice)
set_target_properties(${PROJECT_LIBRARY_NAME} PROPERTIES PREFIX "")


#### build ProtoBot executable
# add ProtoBot executable target

aux_source_directory("src" SRC_ROOT)

add_executable(
    ${PROJECT_EXECUTABLE_NAME}
    ${SRC_ROOT} libprotobot/include/protobot/ddl/bot_options.h)

# add library directories for libpqxx (PostgreSQL) and DPP
#target_link_directories(
#    ${PROJECT_EXECUTABLE_NAME}
#    PUBLIC
#    ${PQXX_LIBRARIES_DIR}
#    ${DPP_LIBRARIES_DIR}
#)

# link executable with ProtoBot library
target_link_libraries(
    ${PROJECT_EXECUTABLE_NAME}
    libprotobot
)


#### add ProtoBot modules
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules)


