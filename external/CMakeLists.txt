# find OpenSSL library
find_package(OpenSSL REQUIRED)

# build CLI11 command-line argument parser
add_subdirectory(CLI11)

# rang (terminal colors etc)
add_subdirectory(rang)

# build AngelScript shared library
SET(BUILD_SHARED_LIBS ON CACHE BOOL "")
add_subdirectory(angelscript/sdk/angelscript/projects/cmake)

# build shared library for libpqxx (PostgreSQL C++ connector)
#SET(BUILD_SHARED_LIBS ON CACHE BOOL "")
#add_subdirectory(libpqxx)

# build sqlpp11 with PostgreSQL support
#find_package(PostgreSQL REQUIRED)
#SET(BUILD_POSTGRESQL_CONNECTOR ON CACHE BOOL "")

include(FetchContent)

FetchContent_Declare(sqlpp11
  GIT_REPOSITORY https://github.com/rbock/sqlpp11
  GIT_TAG        origin/main
)

add_subdirectory(sqlpp11)

# pybind11 (C++ Python bindings)
add_subdirectory(pybind11)

# build spdlog logger shared library
SET(SPDLOG_BUILD_SHARED ON CACHE BOOL "")
add_subdirectory(spdlog)

#add_subdirectory(zlib)

# build D++ Discord library
#SET(THREADS_PREFER_PTHREAD_FLAG OFF CACHE BOOL "")
#add_subdirectory(DPP)
#message("sodium_INCLUDE_DIR: ${sodium_INCLUDE_DIR}")
