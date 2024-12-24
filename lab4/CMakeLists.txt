cmake_minimum_required(VERSION 3.10)
project(Lab4 C)

set(CMAKE_C_STANDARD 99)

# Опции компилятора
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O2")

# Указываем исходные файлы
set(SOURCE_FILES
    lab4/main1.c
    lab4/main2.c
    lab4/derivative.c
    lab4/lib_sort.c
)

# Создаем динамическую библиотеку pi_e
add_library(der SHARED derivative.c)
target_compile_options(der PRIVATE -fPIC)

# Создаем динамическую библиотеку sort
add_library(sort SHARED lib_sort.c)
target_compile_options(sort PRIVATE -fPIC)

# Создаем исполнимые файлы
add_executable(main1 main1.c)
add_executable(main2 main2.c)

# Линкуем библиотеки с исполнимыми файлами
target_link_libraries(main1 der sort m)
target_link_libraries(main2 der sort m)

# Устанавливаем директории с заголовочными файлами
target_include_directories(main1 PRIVATE ${CMAKE_SOURCE_DIR}/lab4)
target_include_directories(main2 PRIVATE ${CMAKE_SOURCE_DIR}/lab4)

# Настройки путей поиска для библиотек
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Для правильной работы с shared библиотеками
set_target_properties(der PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
set_target_properties(sort PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
