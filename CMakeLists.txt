cmake_minimum_required(VERSION 3.13)
project(hss VERSION 0.0.1)

# 设置项目包含目录、库目录和可执行文件目录
include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ROOT 编译选项
if(NOT ROOT_DIR)
    set(ROOT_DIR ${CMAKE_SOURCE_DIR}/root/cmake)
endif()
find_package(ROOT)
set(ROOT_CC_FLAG_LIST ${ROOT_CC_FLAGS})
set(ROOT_CXX_FLAG_LIST ${ROOT_CXX_FLAGS})
separate_arguments(ROOT_CC_FLAG_LIST)
separate_arguments(ROOT_CXX_FLAG_LIST)
add_compile_options(${ROOT_CXX_FLAG_LIST})
add_compile_definitions(${ROOT_DEFINITIONS})
include_directories(${ROOT_INCLUDE_DIRS})

# Delphes 编译选项
if(NOT DELPHES_DIR)
    set(DELPHES_DIR ${CMAKE_SOURCE_DIR}/MG5_aMC/Delphes)
endif()
set(DELPHES_INC_DIR ${DELPHES_DIR} ${DELPHES_DIR}/external)
set(DELPHES_LIB_DIR ${DELPHES_DIR})
include_directories(${DELPHES_INC_DIR})
link_directories(${DELPHES_LIB_DIR})
add_compile_definitions(DELPHES_DIR="${DELPHES_DIR}")
add_compile_definitions(DELPHES_EXTERNAL_DIR="${DELPHES_DIR}/external")
add_compile_definitions(LIBDELPHES_SO="${DELPHES_LIB_DIR}/libDelphes.so")

# ad-part 编译选项
if(NOT AD_PART_DIR)
    set(AD_PART_DIR ${CMAKE_SOURCE_DIR}/ad-part)
endif()
set(AD_PART_INC_DIR ${AD_PART_DIR}/include)
set(AD_PART_LIB_DIR ${AD_PART_DIR}/lib)
include_directories(${AD_PART_INC_DIR})
link_directories(${AD_PART_LIB_DIR})

# 优化、调试和通用编译选项
option(ENABLE_SANITIZER "compile and link sanitizing flags" ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG)
endif()
if(CMAKE_BUILD_TYPE STREQUAL DEBUG)
    add_compile_options(-O0 -fno-omit-frame-pointer)
    if(ENABLE_SANITIZER)
        set(SANITIZER_OPTIONS -fno-sanitize-recover=all)
        list(APPEND SANITIZER_OPTIONS -fsanitize=address)
        list(APPEND SANITIZER_OPTIONS -fsanitize=leak)
        list(APPEND SANITIZER_OPTIONS -fsanitize=undefined)
        list(APPEND SANITIZER_OPTIONS -fsanitize=float-divide-by-zero)
        list(APPEND SANITIZER_OPTIONS -static-libasan)
        add_compile_options(${SANITIZER_OPTIONS})
        add_link_options(${SANITIZER_OPTIONS})
    endif()
else()
    add_compile_options(-O3)
endif()
add_compile_options(-Wall -Wshadow -Wextra)
add_compile_definitions(_GNU_SOURCE)

# 编译环境检查
set(CODE_TO_COMPILE "#include <string>\\nusing std::operator\"\"s;\\n")
set(TEMP_OUTPUT "/tmp/trail_$$.o")
set(TRAIL_FLAGS "-xc++ ${ROOT_CXX_FLAGS} - -o ${TEMP_OUTPUT} -c")
message("compiling trail code...")
execute_process(COMMAND sh -c
"printf '${CODE_TO_COMPILE}' | ${CMAKE_CXX_COMPILER} ${TRAIL_FLAGS} && rm ${TEMP_OUTPUT}"
RESULT_VARIABLE RETCODE)
if(NOT RETCODE EQUAL 0)
    message(FATAL_ERROR "string literals not supported, expect C++14 or later")
endif()
message("passed test for string literal operator support")

# 以下为可执行文件构建

add_executable(nanogen src/nanogen.cpp
ad-part/src/adenv.cpp ad-part/src/adbranch.cpp ad-part/src/adfs.cpp ad-part/src/adnbranch.cpp ad-part/src/admem.cpp)
target_link_libraries(nanogen ROOT::Tree)