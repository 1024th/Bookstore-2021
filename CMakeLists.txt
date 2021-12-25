cmake_minimum_required(VERSION 3.16)
project(Bookstore)

set(CMAKE_CXX_STANDARD 20)
#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)
#add_compile_options(-D MyDebug)

include_directories(
        ${PROJECT_SOURCE_DIR}/src/
        ${PROJECT_SOURCE_DIR}/bin/
)
set(src_dir
#        ${PROJECT_SOURCE_DIR}/src/storage_test.cpp
        ${PROJECT_SOURCE_DIR}/src/command_parser.h
        ${PROJECT_SOURCE_DIR}/src/command_parser.cpp
        ${PROJECT_SOURCE_DIR}/src/user_manager.h
        ${PROJECT_SOURCE_DIR}/src/book_manager.h
        ${PROJECT_SOURCE_DIR}/src/unrolled_linked_list.h
        ${PROJECT_SOURCE_DIR}/src/char.h
        ${PROJECT_SOURCE_DIR}/src/basic_file_io.h
        ${PROJECT_SOURCE_DIR}/src/exceptions.h
        src/user_manager.cpp
        src/block_storage.h
        src/book_manager.cpp
        src/utils.h
        src/main.cpp src/logger.cpp src/bookstore.h src/bookstore.cpp)
add_executable(code ${src_dir})

# 添加链接库
# target_link_libraries(code ${PROJECT_SOURCE_DIR}/bin/libUll.a)

# 后缀名为 .a 和 .so 的文件分别为静态链接库和动态链接库