cmake_minimum_required(VERSION 3.16)

if(ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
endif()

project(modifiedeight_android)
set(CMAKE_CXX_STANDARD 11)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../" ABSOLUTE)

include_directories(${REPO_ROOT}/stb)
include_directories(${REPO_ROOT}/glm)
include_directories(${REPO_ROOT}/OpenAES/inc/)
include_directories(${REPO_ROOT}/RakNet/Source/)

OPTION(JSONCPP_WITH_TESTS "Compile and run JsonCpp test executables" OFF)
OPTION(JSONCPP_WITH_POST_BUILD_UNITTEST "Automatically run unit-tests as a post build step" OFF)
SET(JSONCPP_WITH_TESTS OFF)
SET(JSONCPP_WITH_POST_BUILD_UNITTEST OFF)

include_directories(${REPO_ROOT}/jsoncpp/jsoncpp/include)
add_subdirectory(${REPO_ROOT}/jsoncpp/jsoncpp ${CMAKE_CURRENT_BINARY_DIR}/jsoncpp)
include_directories(${REPO_ROOT}/jsoncpp/jsoncpp/src/lib_json/)

include_directories(${REPO_ROOT}/utf8proc)

add_subdirectory(${REPO_ROOT}/RakNet ${CMAKE_CURRENT_BINARY_DIR}/RakNet)
add_subdirectory(${REPO_ROOT}/utf8proc ${CMAKE_CURRENT_BINARY_DIR}/utf8proc)
add_subdirectory(${REPO_ROOT}/OpenAES ${CMAKE_CURRENT_BINARY_DIR}/OpenAES)

if(CLIENT_TYPE STREQUAL "newadditions")
    add_subdirectory(${REPO_ROOT}/modifiedeight-newadditions ${CMAKE_CURRENT_BINARY_DIR}/modifiedeight-newadditions)
else()
    add_subdirectory(${REPO_ROOT}/modifiedeight ${CMAKE_CURRENT_BINARY_DIR}/modifiedeight)
endif()
