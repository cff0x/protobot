#
# ProtoBot - modular, dynamic and extensible Discord bot
# Author: codefox
# License: MIT
#
#

cmake_minimum_required(VERSION 3.18.4)
set(CMAKE_CXX_STANDARD 20)

# linker flags to use non-apple c++ standard libraries
if(APPLE)
    set(CMAKE_MODULE_LINKER_FLAGS
        "${CMAKE_MODULE_LINKER_FLAGS} -L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib")
endif()

if(WIN32)
    set(WINDOWS_EXPORT_ALL_SYMBOLS ON)
    option(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" ON)
endif()

#### base project settings
# ProtoBot base CMake project file
project("protobot")

# executable + shared library names
set(PROJECT_EXECUTABLE_NAME "${PROJECT_NAME}")
set(PROJECT_LIBRARY_NAME "lib${PROJECT_NAME}")

######## third-party libraries
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

#### threads library
SET(THREADS_PREFER_PTHREAD_FLAG OFF CACHE BOOL "")
find_package(Threads REQUIRED)

add_subdirectory(external)

#find_package(DPP REQUIRED)

# add directories to project
#include_directories(${DPP_INCLUDE_DIR})
#link_directories(${DPP_LIBRARY_DIR})

add_custom_target(generate_events
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/build-tools/event_generator.py ${DPP_INCLUDE_DIR}/cluster.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


######## ProtoBot
#### set main source and include directories
aux_source_directory("libprotobot" LIBRARY_SRC_ROOT)
aux_source_directory("libprotobot/angelscript" LIBRARY_AS_SRC_ROOT)
set(PROTOBOT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libprotobot/include)
include_directories(
        ${PROTOBOT_INCLUDE_DIR}
        ${PostgreSQL_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/external/CLI11/include
        ${CMAKE_SOURCE_DIR}/external/rang/include
        ${CMAKE_SOURCE_DIR}/external/angelscript/sdk/angelscript/include)

#### build ProtoBot shared library
# add ProtoBot library target
add_library(
    ${PROJECT_LIBRARY_NAME}
    SHARED
    ${LIBRARY_SRC_ROOT}
    ${LIBRARY_AS_SRC_ROOT}
)

# link ProtoBot library with third-party libraries
target_link_libraries(
    ${PROJECT_LIBRARY_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENSSL_CRYPTO_LIBRARY}
    ${OPENSSL_SSL_LIBRARY}
    ${ANGELSCRIPT_LIBRARY_NAME}
    dpp
    spdlog
    pq
    pqxx
    sqlpp11::sqlpp11
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


#### CPack package
install(TARGETS ${PROJECT_EXECUTABLE_NAME} ${PROJECT_LIBRARY_NAME} spdlog
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(DIRECTORY libprotobot/include/ DESTINATION include/libprotobot)
install(DIRECTORY data/config DESTINATION bin)
install(FILES data/run.sh DESTINATION bin)

set(CPACK_PACKAGE_DESCRIPTION "ProtoBot")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "420")
set(CPACK_PACKAGE_VERSION_PATCH "69")
set(CPACK_PACKAGE_VENDOR "cff0x")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-dev")
include (CPack)