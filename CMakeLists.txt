cmake_minimum_required(VERSION 3.20)
project(rtype)
set(CMAKE_CXX_STANDARD 20)
if (WIN32)
        set(CONAN_DISABLE_CHECK_COMPILER OFF)
        add_definitions(-D_WIN32_WINNT=0x0A00)
endif()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
add_subdirectory(server)
add_subdirectory(client)
file(COPY assets DESTINATION ${PROJECT_SOURCE_DIR}/bin)
add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/assets
        ${PROJECT_SOURCE_DIR}/bin/assets
)
