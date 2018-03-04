cmake_minimum_required(VERSION 3.9)
project(assignment1)

set(CMAKE_CXX_STANDARD 14)

include_directories("include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_executable(indexer src/main_indexer.cpp src/parser.cpp include/parser.hpp include/file_manager.hpp src/file_manager.cpp include/doc_preprocessor.hpp src/doc_preprocessor.cpp include/defs.hpp include/tokenizer.hpp src/tokenizer.cpp src/porter_stemmer.cpp)
#add_executable(searcher src/main_searcher.cpp)
