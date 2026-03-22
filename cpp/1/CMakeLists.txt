cmake_minimum_required(VERSION 3.22)
project(lab1) # Название всего проекта

# set - устанавливает переменные (принимает имя и значение)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Строгое соблюдение стандарта

# подключение библиотеки!
add_subdirectory(libs/utils)
# принимает флаги, использующиеся при компиляции
#add_compile_options(-Wall -Wextra -Werror -pedantic)

# Добавление пути до исполняемого файла!!
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


# создает исполняемый файл - принимает Имя target и список исходных файлов (ТОЛЬКО .cpp файлы!)
add_executable(lab1 src/main.cpp
        src/funcs_arr_to_toml.cpp
        src/funcs_toml_to_arr.cpp
        src/exception.cpp
        src/ui.cpp
)

# Линкуем основную программу с библиотекой утилит
target_link_libraries(lab1 PRIVATE utils)


# Добавление директории для заголовков (PRIVATE - видит только этот target, PUBLIC - видят все)
target_include_directories(lab1 PRIVATE include
        libs/utils/include)
