# parent project for the protobot modules
project("ProtobotModules")
message(STATUS "====== Loading ProtoBot module projects ======")

# set module library output path to ${buildDir}/modules (based on the parent directory of this parent project)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
message(STATUS "-> Built module libraries will be written to: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

# load protobot modules from subdirectories in modules folder
set(MODULES_DIR ${PROJECT_SOURCE_DIR})
file(GLOB MODULE_DIR_LIST ${MODULES_DIR}/*)
foreach (FULL_MODULE_NAME ${MODULE_DIR_LIST})

  # make sure the found entry is a directory (directories have no extension :3)
  get_filename_component(FILE_EXT ${FULL_MODULE_NAME} EXT)
  if("${FILE_EXT}" STREQUAL "")

    # get module name and output file name
    get_filename_component(MODULE_NAME ${FULL_MODULE_NAME} NAME)
    message(STATUS "--> Found module ${MODULE_NAME} at ${FULL_MODULE_NAME}")

    # load module cmake project
    add_subdirectory(${FULL_MODULE_NAME})

    # set output filename (<module_name>.protomod.<library extension>)
    set_target_properties(${MODULE_NAME} PROPERTIES OUTPUT_NAME "${MODULE_NAME}.protomod")
    set_target_properties(${MODULE_NAME} PROPERTIES PREFIX "")
    set_target_properties(${MODULE_NAME} PROPERTIES SUFFIX "")

    install(TARGETS ${MODULE_NAME}
            RUNTIME DESTINATION bin/modules
            LIBRARY DESTINATION bin/modules
            ARCHIVE DESTINATION bin/modules)

  endif()

endforeach(FULL_MODULE_NAME)

message(STATUS "====== Done loading ProtoBot module projects ======")
