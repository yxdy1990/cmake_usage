# cmake test

# 1. cmake version
cmake_minimum_required(VERSION 2.8)

# 2. project name
project(cmake_test)

# 3. header file path
include_directories(
include
)

# 4. library file path
#set(CMAKE_USE_RELATIVE_PATHS on)
#find_library(LIB_DIRS ${PROJECT_SOURCE_DIR}/lib NO_DEFAULT_PATH)
link_directories(${PROJECT_SOURCE_DIR}/lib)

# 5. source file path
aux_source_directory(src DIR_SRCS)

# 6. set environment variable
set(TEST_MAIN ${DIR_SRCS})
set(EXE_NAME gateway)

# 7. add executable file
ADD_EXECUTABLE(${EXE_NAME} ${TEST_MAIN})

# 8. add link library
target_link_libraries(${EXE_NAME} pthread mylib)

# 9. set complier
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# 10. set output path
set(EXECUTABLE_OUTPUT_PATH ../bin)

