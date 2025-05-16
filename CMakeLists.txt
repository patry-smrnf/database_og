cmake_minimum_required(VERSION 3.30)
project(database_OG)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG 11.1.4
)

FetchContent_MakeAvailable(fmt)


add_executable(database_OG main.cpp
        Misc/app.cpp
        Misc/app.h
        Databases/database.cpp
        Databases/database.h
        Databases/database_creator.cpp
        Databases/database_structures.h
        Databases/database_reader.cpp
        Databases/database_methods.cpp
        query/query.cpp
        query/query.h
        query/query_structures.h)
target_link_libraries(database_OG fmt)