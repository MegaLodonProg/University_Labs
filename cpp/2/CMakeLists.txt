cmake_minimum_required(VERSION 3.22)
project(lab2) # Название всего проекта

# set - устанавливает переменные (принимает имя и значение) - потом обращение через $
# Настройки компилятора
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Строгое соблюдение стандарта
set(CMAKE_CXX_EXTENSIONS OFF) # Запрет на использование расширенной версии

# Добавление путей до исполняемых файлов!!
# Устанавливает каталог для выходных исполняемых файлов (bin в build директории)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Устанавливает каталог для статических библиотек (.a, .lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Ищем предустановленный GoogleTest, REQUIRED - transl : необходимый, если не найден, сборка прервется с ошибкой
find_package(GTest REQUIRED)

# Добавляем нашу библиотеку как статическую
# StudentWorkLib - имя target'а библиотеки; src/MyClass.cpp - исходные файлы библиотеки
add_library(StudentWorkLib STATIC
        src/lib/StudentWork.cpp
        src/lib/WorkStack.cpp
)

# Подключаем заголовочные файлы
# PUBLIC - заголовки будут видны и самой библиотеке, и всем кто ее линкует
target_include_directories(StudentWorkLib PUBLIC include/)

# создает исполняемый файл - принимает Имя target и список исходных файлов (ТОЛЬКО .cpp файлы!)
add_executable(lab2 src/app/main.cpp)
# Линкует исполняемый файл с нашей библиотекой
# lab2 будет использовать функциональность StudentWorkLib
target_link_libraries(lab2 StudentWorkLib)

# Тесты (отдельный CMake в папке tests)
add_subdirectory(tests)
