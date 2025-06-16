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

    # bzip2
    # cairo-fc
    # cairo-ft
    # cairo-gobject
    # cairo-pdf
    # cairo-png
    # cairo-ps
    # cairo-svg
    # cairo-win32-font
    # cairo-win32
    # cairo
    # dav1d
    # # expat
    # flac
    # fontconfig
    # freetype2
    # fribidi
    # gdk-pixbuf-2.0
    # gio-2.0
    # gio-windows-2.0
    # glib-2.0
    # gmodule-2.0
    # gmodule-export-2.0
    # gmodule-no-export-2.0
    # gobject-2.0
    # gobject-introspection-1.0
    # gobject-introspection-no-export-1.0
    # graphene-1.0
    # graphene-gobject-1.0
    # gst-editing-services-1.0
    # gstreamer-1.0
    
    # gstreamer-allocators-1.0
    # ###### gstreamer-analytics-1.0
    # gstreamer-app-1.0
    # gstreamer-audio-1.0
    # gstreamer-bad-audio-1.0
    # gstreamer-base-1.0
    # gstreamer-check-1.0
    # gstreamer-codecparsers-1.0
    # gstreamer-controller-1.0
    # gstreamer-cuda-1.0
    # gstreamer-d3d11-1.0

    # ## gstreamer-d3d12-1.0
    # gstreamer-fft-1.0
    # gstreamer-gl-1.0
    # gstreamer-gl-prototypes-1.0
    # gstreamer-insertbin-1.0
    # gstreamer-mpegts-1.0
    # gstreamer-mse-1.0
    # gstreamer-net-1.0
    # gstreamer-pbutils-1.0
    # gstreamer-photography-1.0
    # gstreamer-play-1.0
    # gstreamer-player-1.0
    # gstreamer-plugins-bad-1.0
    # gstreamer-plugins-base-1.0
    # gstreamer-riff-1.0
    # gstreamer-rtp-1.0
    # gstreamer-rtsp-1.0
    # gstreamer-rtsp-server-1.0
    # gstreamer-sctp-1.0
    # gstreamer-sdp-1.0
    # gstreamer-tag-1.0
    # gstreamer-transcoder-1.0
    # gstreamer-validate-1.0
    # gstreamer-video-1.0
    # gstreamer-webrtc-1.0
    # gstreamer-webrtc-nice-1.0
    # gthread-2.0
    # harfbuzz-gobject
    # harfbuzz
    # json-glib-1.0
    # libass
    # libavcodec
    # libavfilter
    # libavformat
    # libavutil
    # libcrypto
    # libdca
    # libdts
    # libdv
    # libffi
    # libjpeg
    # libmpg123
    # ## libnghttp2
    # libopenjp2
    # libpcre2-8
    # libpng
    # libpng16
    # libpsl
    # # #libqrencode
    # librsvg-2.0
    # librtmp
    # libsoup-3.0
    # libsrtp2
    # libssl
    # libswresample
    # libswscale
    # libtiff-4
    # libxml-2.0
    # ltc
    # nice
    # ogg
    # opencore-amrnb
    # opencore-amrwb
    # openh264
    # openssl
    # opus
    # orc-0.4
    # pango
    # pangocairo
    # pangowin32
    # pixman-1
    # py3cairo
    # pygobject-3.0
    # sbc
    # ## soundtouch
    # spandsp
    # # speex
    # # sqlite3
    # srt
    # SvtAv1Enc
    # SvtJpegxs
    # taglib
    # theora
    # theoradec
    # theoraenc
    # vo-aacenc
    # vorbis
    # vorbisenc
    # vorbisfile
    # ## vpx
    # wavpack
    # ## webrtc-audio-processing-2
    # x264
    # x265
    # zbar
    # zlib
    

    # gstreamer-base-1.0
    # gstreamer-video-1.0
    # gstreamer-app-1.0
    # gobject-2.0
    # glib-2.0
    # gstreamer-plugins-base-1.0
    # gstreamer-rtp-1.0
    # gstreamer-plugins-bad-1.0
    # gstreamer-player-1.0
    # gstreamer-sdp-1.0

    # gstreamer-plugins-good-1.0-0
    # gstreamer-libav-1.0
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
