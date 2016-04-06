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

include_directories(SYSTEM ${Boost_INCLUDE_DIR} ${HDF5_INCLUDE_DIRS})

add_definitions( -DH5_USE_BOOST -DMVD_VERSION_MAJOR=\"${VERSION_MAJOR}\" -DMVD_VERSION_MINOR=\"${VERSION_MINOR}\" )

aux_source_directory(src SRC_LIST)
add_executable(synap-tool ${SRC_LIST} ${MVD_HEADERS_BITS} ${MVD_HEADERS})
target_link_libraries(mvd-tool ${HDF5_C_LIBRARIES} ${HDF5_HL_LIBRARIES})

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/mvd DESTINATION ${INCLUDE_INSTALL_DIR})
install(TARGETS mvd-tool RUNTIME DESTINATION ${BIN_INSTALL_DIR})

add_subdirectory(tests/unit)
