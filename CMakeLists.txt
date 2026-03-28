cmake_minimum_required(VERSION 3.20)
project(pixellab VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(pixellab
    src/main.cpp
    src/App.cpp
    src/Button.cpp
    src/Histogram.cpp
)

target_include_directories(pixellab
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

target_link_libraries(pixellab
    PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
)

# Windows only: copy SDL3 runtime dll beside exe
if(WIN32)
    add_custom_command(
        TARGET pixellab POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SDL3::SDL3>
                $<TARGET_FILE_DIR:pixellab>
        VERBATIM
    )
endif()
