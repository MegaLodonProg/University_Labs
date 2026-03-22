cmake_minimum_required(VERSION 3.22)
project(LAB3)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/third_party)

# Скачивание необходимых библиотек (json.hpp в third_party)
set(JSON_URL "https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp")
set(JSON_PATH "${CMAKE_SOURCE_DIR}/third_party/json.hpp")

if(NOT EXISTS ${JSON_PATH})
    file(DOWNLOAD ${JSON_URL} ${JSON_PATH})
endif()

# Скачивание FTXUI библиотеки (для псевдографики)
include(FetchContent)

FetchContent_Declare(
        ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG v6.1.9
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/ftxui
)
FetchContent_MakeAvailable(ftxui)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(GTest REQUIRED)
find_package(Boost REQUIRED)

add_library(CoreLib
        model/src/Worker.cpp
        model/src/Workman.cpp
        model/src/Master.cpp
        model/src/WorkZone.cpp
        model/src/Brigade.cpp
)

target_include_directories(CoreLib PUBLIC model/include)
target_link_libraries(CoreLib PRIVATE Boost::headers)

# Тесты
add_subdirectory(tests)

add_library(ViewLib
        view/src/MainScreen.cpp
        view/src/ActionsScreen.cpp
        view/src/StatsScreen.cpp
        view/src/WorkersScreen.cpp
        view/src/WorkZonesScreen.cpp
        view/src/ImportExportScreen.cpp
)

target_include_directories(ViewLib PUBLIC
        view/include
        model/include
)

target_link_libraries(ViewLib PUBLIC
        CoreLib
        ftxui::component
        ftxui::dom
        ftxui::screen
)

add_executable(lab3 my_app/main.cpp)

add_executable(parallel_test_brigade
        parallel_test/main_parallel_test.cpp
        model/src/Brigade.cpp
        model/src/Worker.cpp
        model/src/Workman.cpp
        model/src/Master.cpp
        model/src/WorkZone.cpp
)

target_include_directories(parallel_test_brigade PRIVATE model/include)
target_link_libraries(parallel_test_brigade Boost::boost)

target_link_libraries(lab3 PRIVATE ViewLib)