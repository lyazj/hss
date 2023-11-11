cmake_minimum_required(VERSION 3.13)
project(hss VERSION 0.0.1)

# 设置项目包含目录、库目录和可执行文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ROOT 编译选项
if(NOT ROOT_DIR)
    set(ROOT_DIR ${PROJECT_SOURCE_DIR}/root/cmake)
endif()
find_package(ROOT)
set(ROOT_CC_FLAG_LIST ${ROOT_CC_FLAGS})
set(ROOT_CXX_FLAG_LIST ${ROOT_CXX_FLAGS})
separate_arguments(ROOT_CC_FLAG_LIST)
separate_arguments(ROOT_CXX_FLAG_LIST)
add_compile_options(${ROOT_CXX_FLAG_LIST})
add_compile_definitions(${ROOT_DEFINITIONS})
include_directories(${ROOT_INCLUDE_DIRS})
link_directories(${ROOT_LIBRARY_DIR})

# Delphes 编译选项
if(NOT DELPHES_DIR)
    set(DELPHES_DIR ${PROJECT_SOURCE_DIR}/MG5_aMC/Delphes)
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
    set(AD_PART_DIR ${PROJECT_SOURCE_DIR}/ad-part)
endif()
set(AD_PART_INC_DIR ${AD_PART_DIR}/include)
set(AD_PART_LIB_DIR ${AD_PART_DIR}/lib)
include_directories(${AD_PART_INC_DIR})
link_directories(${AD_PART_LIB_DIR})

# 优化、调试和通用编译选项
option(ENABLE_SANITIZER "compile and link with sanitizing flags" OFF)
option(ENABLE_GPROF "compile and link with gprof flags" OFF)
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
        add_compile_options(${SANITIZER_OPTIONS})
        add_link_options(${SANITIZER_OPTIONS})
    endif()
else()
    add_compile_options(-O3)
endif()
if(ENABLE_GPROF)
    set(GPROF_OPTIONS -pg)
    add_compile_options(${GPROF_OPTIONS})
    add_link_options(${GPROF_OPTIONS})
endif()
add_compile_options(-Wall -Wshadow -Wextra)
add_compile_definitions(_GNU_SOURCE)

# 可选：CMSSW 环境
if(CMSSW_RLSE)
    include_directories(/cvmfs/cms.cern.ch/slc7_amd64_gcc700/cms/cmssw/${CMSSW_RLSE}/src)
    link_directories(/cvmfs/cms.cern.ch/slc7_amd64_gcc700/cms/cmssw/${CMSSW_RLSE}/lib/slc7_amd64_gcc700)
    if(NOT FWLITE_LIBRARIES)
        set(FWLITE_LIBRARIES FWCoreFWLite DataFormatsFWLite)
    endif()
    add_compile_definitions(CMSSW_RLSE=${CMSSW_RLSE})
endif()

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

# 以下为子项目构建

SET(AD_NOONNX ON)
SET(AD_NOEXEC ON)
SET(AD_NOPDG ON)
add_subdirectory(${AD_PART_DIR})

# 以下为静态库文件构建

file(GLOB CMS_SOURCES src/cms/*.cpp)
add_library(cms ${CMS_SOURCES})
file(GLOB HSS_SOURCES src/hss/*.cpp)
add_library(hss ${HSS_SOURCES})

# 以下为可执行文件构建

add_executable(nanogen-plot src/nanogen-plot.cpp)
target_link_libraries(nanogen-plot adhep cms ROOT::Tree ROOT::Gpad ASImage)

add_executable(nanogen-dump src/nanogen-dump.cpp)
target_link_libraries(nanogen-dump adhep ROOT::Tree)

add_executable(merge-tree src/merge-tree.cpp)
target_link_libraries(merge-tree ${FWLITE_LIBRARIES} ROOT::Tree)

add_executable(nanoaod-plot src/nanoaod-plot.cpp)
target_link_libraries(nanoaod-plot adhep cms ROOT::Tree ROOT::Gpad ASImage)

add_executable(trace-root src/trace-root.cpp)
target_link_libraries(trace-root hss)

add_executable(hist-add src/hist-add.cpp)
target_link_libraries(hist-add adhep ROOT::Gpad)
