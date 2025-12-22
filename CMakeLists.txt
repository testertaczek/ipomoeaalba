cmake_minimum_required(VERSION 3.31)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(GitVersion)
get_git_version(
    IA_VERSION
    IA_VERSION_FULL
    "${CMAKE_CURRENT_LIST_DIR}"
)

project(ipomoea_alba 
    VERSION 0.1 
    LANGUAGES C CXX
)

if (MSVC)
    message(FATAL_ERROR "MSVC support is very much questionable and work in progress. Use GCC or Clang.")
endif()

# Don't use absolute paths to the build tree in RPATH, this makes the build tree relocatable
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)

# Export the compile datebase as a json file, this can be used by VIM language server
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable placing targets into a hierarchy for IDE generators
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# C settings
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# C++ settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Engine headers
add_library(ia_headers INTERFACE)
add_library(ia::headers ALIAS ia_headers)

target_include_directories(ia_headers
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_definitions(ia_headers
    INTERFACE
        IA_VERSION="${IA_VERSION}"
        IA_VERSION_FULL="${IA_VERSION_FULL}"
)
set_target_properties(ia_headers PROPERTIES
    FOLDER "engine"
)

# Detect assembly configuration
set(IA_ASM_ARCH "")
if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    set(IA_ASM_ARCH amd64)
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|ARM64")
    set(IA_ASM_ARCH aarch64)
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "riscv64")
    set(IA_ASM_ARCH rv64gc)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(IA_ASM_ARCH wasm)
else()
    message(FATAL_ERROR "Unsupported CPU architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(IA_ASM_PLATFORM "")
if (IA_ASM_ARCH STREQUAL "aarch64")
    set(IA_ASM_PLATFORM aapcs)
elseif (IA_ASM_ARCH STREQUAL "wasm")
    set(IA_ASM_PLATFORM js)
else()
    if (WIN32)
        set(IA_ASM_PLATFORM ms)
    else()
        set(IA_ASM_PLATFORM sysv)
    endif()
endif()

set(IA_ASM_ABI "")
if (WIN32)
    set(IA_ASM_ABI pe)
elseif (APPLE)
    set(IA_ASM_ABI macho)
elseif (UNIX)
    set(IA_ASM_ABI elf)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(IA_ASM_ABI ems)
endif()

set(IA_ASM_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/source/engine/asm/fcontext_${IA_ASM_ARCH}_${IA_ASM_PLATFORM}_${IA_ASM_ABI}.s
)
if (NOT EXISTS ${IA_ASM_SOURCE})
    message(FATAL_ERROR "Missing assembly sources:\n ${IA_ASM_SOURCE}")
endif()

# Engine foundation library (core, compute, datastructures)
add_library(ia_foundation OBJECT
    source/engine/datastructures.c
    source/engine/foundation.c
    source/engine/system_linux.c # TODO
    source/engine/system_unix.c # TODO
    ${IA_ASM_SOURCE}
)
add_library(ia::foundation ALIAS ia_foundation)
target_link_libraries(ia_foundation PRIVATE ia::headers)
set_target_properties(ia_foundation PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    FOLDER "engine"
)

add_executable(cynicmoon
    source/cynicmoon/main.c
    $<TARGET_OBJECTS:ia_foundation>
)
target_link_libraries(cynicmoon
    PRIVATE
        ia::headers
)
set_target_properties(cynicmoon PROPERTIES
    FOLDER "source/cynicmoon"
)
