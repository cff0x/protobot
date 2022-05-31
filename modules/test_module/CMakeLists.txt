# MODULE_NAME is set by the parent CMakeLists.txt (based on the module project folder)
project(${MODULE_NAME})

# include directories
set(PROTOBOT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(WINDOWS_EXPORT_ALL_SYMBOLS ON)

# build shared module library
aux_source_directory("src" modsrc)
add_library(${MODULE_NAME} SHARED ${modsrc})

# add protobot include directory
target_include_directories(
        ${MODULE_NAME}
        PUBLIC
        ${PROTOBOT_INCLUDE_DIR}
)

# link against protobot shared library
target_link_libraries(
    ${MODULE_NAME}
     libprotobot
)
