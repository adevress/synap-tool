project(synap-tool)
cmake_minimum_required(VERSION 2.8)

enable_testing()

set(VERSION_MAJOR "0")
set(VERSION_MINOR "1")

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
      ${PROJECT_SOURCE_DIR}/CMake/portability
      ${PROJECT_SOURCE_DIR}/CMake/packages
      ${PROJECT_SOURCE_DIR}/CMake/config)

include(DefineInstallationPaths)
include(ReleaseDebugAutoFlags)


option(UNIT_TESTS "Enable or disable unit tests" ON)
set(Boost_NO_BOOST_CMAKE TRUE)

## find dependencies
find_package(Boost 1.41 REQUIRED COMPONENTS system)
find_package(HDF5 REQUIRED)
find_package(Threads)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/highfive/include )
include_directories(SYSTEM ${Boost_INCLUDE_DIR} ${HDF5_INCLUDE_DIRS} )

add_definitions( -std=c++11 -DH5_USE_BOOST )

aux_source_directory(src SRC_LIST)
add_executable(synap-tool ${SRC_LIST})
target_link_libraries(synap-tool ${HDF5_C_LIBRARIES} ${HDF5_HL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})


install(TARGETS synap-tool RUNTIME DESTINATION ${BIN_INSTALL_DIR})

add_subdirectory(tests/unit)
