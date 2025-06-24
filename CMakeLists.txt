cmake_minimum_required(VERSION 3.10)
project(gstreamer_godot)

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
message(STATUS "Директории заголовочных файлов GStreamer: ${GST_INCLUDE_DIRS}")

### 3. Настройка самого GDExtension
set(SOURCES
    src/gstreamer.cpp
    src/my_singleton.cpp
    src/register_types.cpp
)

add_library(gstreamer_godot SHARED ${SOURCES})

target_include_directories(gstreamer_godot PRIVATE
    ${GODOT_CPP_DIR}/include
    ${GODOT_CPP_DIR}/include/core
    ${GODOT_CPP_DIR}/include/gen
)

# Линкуем зависимости: godot-cpp и все найденные библиотеки GStreamer (включая плагины)
target_link_libraries(gstreamer_godot PUBLIC
    godot-cpp
    ${GST_LIBRARIES}
)

### 4. Дополнительные настройки для разных ОС
if(WIN32)
    target_compile_definitions(gstreamer_godot PRIVATE WIN32 _WINDOWS)
    set_target_properties(gstreamer_godot PROPERTIES
        OUTPUT_NAME "libgstreamer_godot.windows.debug.x86_64"
        SUFFIX ".dll"
        PREFIX ""
    )
elseif(LINUX)

    target_compile_definitions(gstreamer_godot PRIVATE LINUX)
    set_target_properties(gstreamer_godot PROPERTIES
        OUTPUT_NAME "gstreamer_godot.linux.debug.x86_64"
        SUFFIX ".so"
        PREFIX ""
    )
endif()

### 5. Копирование скомпилированной библиотеки в папку проекта Godot
add_custom_command(TARGET gstreamer_godot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/project/addons/gstreamer_godot/bin/
    COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:gstreamer_godot>
            ${CMAKE_SOURCE_DIR}/project/addons/gstreamer_godot/bin/
)

# add_custom_command(TARGET gstreamer_godot POST_BUILD
#     # COMMAND ldd $<TARGET_FILE:gstreamer_godot> | grep "=> /" | awk "{print \$3}" > ${CMAKE_SOURCE_DIR}/ldd_libs.txt
#     # COMMAND cat ${CMAKE_SOURCE_DIR}/ldd_libs.txt
#     COMMAND xargs -a ${CMAKE_SOURCE_DIR}/ldd_libs.txt -I '{}' cp -u '{}' ${CMAKE_SOURCE_DIR}/project/addons/gstreamer_godot/bin/
# )