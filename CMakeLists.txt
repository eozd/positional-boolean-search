cmake_minimum_required(VERSION 3.9)
project(assignment1)

set(CMAKE_CXX_STANDARD 14)

include_directories("include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3")

add_library(common STATIC src/file_manager.cpp src/tokenizer.cpp src/porter_stemmer.cpp src/util.cpp src/doc_preprocessor.cpp)

add_executable(indexer src/main_indexer.cpp src/parser.cpp)
set_target_properties(indexer PROPERTIES RUNTIME_OUTPUT_DIRECTORY ..)

add_executable(searcher src/main_searcher.cpp src/query_processor.cpp)
set_target_properties(searcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY ..)

target_link_libraries(indexer common)
target_link_libraries(searcher common)
