cmake_minimum_required(VERSION 3.16)
project(UninstallMe)

set(CMAKE_CXX_STANDARD 17)

set(SFML_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML")

# Tell CMake where SFML is
set(SFML_DIR "${SFML_ROOT_DIR}/lib/cmake/SFML")

find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

add_executable(UninstallMe
    src/main.cpp
)

target_include_directories(UninstallMe PRIVATE
    libs/imgui
)

target_link_libraries(UninstallMe
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

if(MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(UninstallMe PRIVATE
        -static-libgcc
        -static-libstdc++
    )
endif()

file(GLOB SFML_RUNTIME_DLLS CONFIGURE_DEPENDS "${SFML_ROOT_DIR}/bin/*.dll")

if(MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    get_filename_component(GCC_RUNTIME_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    set(MINGW_RUNTIME_DLLS
        "${GCC_RUNTIME_DIR}/libstdc++-6.dll"
        "${GCC_RUNTIME_DIR}/libgcc_s_seh-1.dll"
        "${GCC_RUNTIME_DIR}/libwinpthread-1.dll"
    )
endif()

add_custom_command(TARGET UninstallMe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SFML_RUNTIME_DLLS}
            $<TARGET_FILE_DIR:UninstallMe>
)

if(MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_custom_command(TARGET UninstallMe POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${MINGW_RUNTIME_DLLS}
                $<TARGET_FILE_DIR:UninstallMe>
    )
endif()