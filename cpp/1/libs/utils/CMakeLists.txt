# Библиотека утилит
add_library(utils STATIC src/safe_input.cpp
)

# Публичные include директории библиотеки
target_include_directories(utils PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Стандарт C++ и флаги компиляции
set_target_properties(utils PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)