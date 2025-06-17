cmake_minimum_required(VERSION 3.10)
project(example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(PKG_CONFIG_USE_ STATIC_LIBS ON)


if(MSVC)
    add_compile_options(/std:c++17)
endif()

### 1. Настройка godot-cpp
set(GODOT_CPP_DIR "godot-cpp")  # Путь к godot-cpp
add_subdirectory(${GODOT_CPP_DIR})

### 2. Настройка GStreamer с использованием pkg-config
# Обязательно установите pkg-config для Windows и убедитесь,
# что переменная окружения PKG_CONFIG_PATH указывает на каталог:
#     C:\gstreamer\1.0\msvc_x86_64\lib\pkgconfig
find_package(PkgConfig REQUIRED)

pkg_check_modules(GST REQUIRED
    gstreamer-1.0
    gstreamer-app-1.0
)

if(NOT GST_FOUND)
    message(FATAL_ERROR "Не удалось найти GStreamer. Проверьте установку и PKG_CONFIG_PATH")
endif()

include_directories(${GST_INCLUDE_DIRS})
link_directories(${GST_LIBRARY_DIRS})

message(STATUS "GStreamer библиотеки: ${GST_LIBRARIES}")
message(STATUS "GStreamer include dirs: ${GST_INCLUDE_DIRS}")

### 3. Настройка GDExtension (модуля)
set(SOURCES
    src/my_node.cpp
    src/my_singleton.cpp
    src/register_types.cpp
)

add_library(example SHARED ${SOURCES})

target_include_directories(example PRIVATE
    ${GODOT_CPP_DIR}/include
    ${GODOT_CPP_DIR}/include/core
    ${GODOT_CPP_DIR}/include/gen
)

# Линкуем зависимости: godot-cpp и все найденные библиотеки GStreamer (включая плагины)
target_link_libraries(example PUBLIC
    godot-cpp
    ${GST_LIBRARIES}
)

### 4. Дополнительные настройки для Windows
if(WIN32)
    target_compile_definitions(example PRIVATE WIN32 _WINDOWS)
    set_target_properties(example PROPERTIES
        OUTPUT_NAME "libexample.windows.debug.x86_64"
        SUFFIX ".dll"
        PREFIX ""
    )
elseif(LINUX)

    target_compile_definitions(example PRIVATE LINUX)
    set_target_properties(example PROPERTIES
        OUTPUT_NAME "libexample.linux.debug.x86_64"
        SUFFIX ".so"
        PREFIX ""
    )
endif()

### 5. Копирование скомпилированной библиотеки в папку проекта Godot
add_custom_command(TARGET example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:example>
            ${CMAKE_SOURCE_DIR}/project/addons/example/bin/
)
