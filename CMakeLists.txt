cmake_minimum_required(VERSION 3.16.0)

project(ciot_c)

if(DEFINED ENV{IDF_PATH})
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
else()
    FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

    include_directories(AFTER
        ${CMAKE_SOURCE_DIR}/src/data
        ${CMAKE_SOURCE_DIR}/src/infra
        ${CMAKE_SOURCE_DIR}/src/interfaces
        ${CMAKE_SOURCE_DIR}/src/mongoose
        ${CMAKE_SOURCE_DIR}/src
    )

    add_executable(${PROJECT_NAME} ${app_sources})

    if(WIN32)
        target_link_libraries(${PROJECT_NAME} ws2_32)
    endif()
endif()
