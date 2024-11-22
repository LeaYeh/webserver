if(NOT WIN32)
    string(ASCII 27 Esc)
    set(RESET       "${Esc}[m")
    set(BOLD        "${Esc}[1m")
    set(RED         "${Esc}[31m")
    set(GREEN       "${Esc}[32m")
    set(YELLOW      "${Esc}[33m")
    set(BLUE        "${Esc}[34m")
    set(MAGENTA     "${Esc}[35m")
    set(CYAN        "${Esc}[36m")
    set(WHITE       "${Esc}[37m")
    set(GRAY        "${Esc}[90m")
endif()

cmake_minimum_required(VERSION 3.22.1)
project(webserv LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_COLOR_MAKEFILE ON)

function(debug_message)
    message(STATUS "${GRAY}${ARGV}${RESET}")
endfunction()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_DEPENDS_IN_PROJECT_ONLY ON)
set(CMAKE_CXX_DEPENDS_USE_COMPILER ON)

add_compile_options(-Wall -Wextra -Werror)
add_compile_options("$<$<CONFIG:Debug>:-gdwarf-4>")

set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/source)
set(INC_DIR ${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.cpp)

set(INCLUDE_FOLDERS "${INC_DIR}")

file(GLOB_RECURSE ALL_PATHS ${INC_DIR}/*)

foreach(path ${ALL_PATHS})
    get_filename_component(dir_path ${path} DIRECTORY)
    list(FIND INCLUDE_FOLDERS ${dir_path} index)
    if(${index} EQUAL -1)
        list(APPEND INCLUDE_FOLDERS ${dir_path})
    endif()
endforeach()

debug_message("Source directory: ${SRC_DIR}")
debug_message("Include directory: ${INC_DIR}")
debug_message("Include subdirectories:")
foreach(folder ${INCLUDE_FOLDERS})
    debug_message("  ${folder}")
endforeach()

add_executable(${PROJECT_NAME} ${SOURCES})

set_property(TARGET ${PROJECT_NAME} PROPERTY OBJECT_DEPENDS "${HEADERS}")

foreach(folder ${INCLUDE_FOLDERS})
    target_include_directories(${PROJECT_NAME} PUBLIC ${folder})
endforeach()

target_link_libraries(${PROJECT_NAME} stdc++)
