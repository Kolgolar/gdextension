#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

#include <gst/app/gstappsink.h>
#include "gstreamer.hpp"

#include <regex>
#include <iostream>
#include <string>


using namespace godot;

void GStreamer::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("start_sink_stream"), &GStreamer::start_sink_stream);
    ClassDB::bind_method(D_METHOD("get_sink_image"), &GStreamer::get_sink_image);
	ClassDB::bind_method(D_METHOD("stop_sink_stream"), &GStreamer::stop_sink_stream);
}

GStreamer::GStreamer()
{
    UtilityFunctions::print("GStreamer addon is ready! Yay!");
}

// Завершаем все потоки, если нода уничтожается
GStreamer::~GStreamer()
{
    for (auto it = pipelines.begin(); it != pipelines.end(); ) {
        int port = it->first;

        if (pipelines.count(port)) {
            stop_sink_stream(port);
            it = pipelines.begin();
        } else {
            ++it;
        }
    }
}


void GStreamer::start_sink_stream(godot::String pipeline_desc) {
    std::string pipeline_desc_converted = pipeline_desc.utf8().get_data();
    std::regex port_regex(R"(port\s*=\s*(\d+))");
    std::smatch match;

    int port = -1;
    if (std::regex_search(pipeline_desc_converted, match, port_regex) && match.size() > 1) {
        port = std::stoi(match[1]);
    } else {
        UtilityFunctions::print("Не обнаружен порт в указанном пайплайне.");
        return;
    }

    setenv("GST_DEBUG", "3", 1); // Включаем подробный дебаг gstreamer
    gst_init(nullptr, nullptr);

    if (pipelines.count(port)) {
        UtilityFunctions::push_warning("Пайплайн на этом порту уже существует.");
        return;
    }

    GError *error = nullptr;
    GstElement *pipeline = gst_parse_launch(pipeline_desc_converted.c_str(), &error);
    if (!pipeline) {
        UtilityFunctions::printerr("Не удалось создать пайплайн.");
        return;
    }

    GstElement *appsink = gst_bin_get_by_name(GST_BIN(pipeline), "appsink");
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    pipelines[port] = pipeline;
    appsinks[port] = appsink;
}


Ref<Image> GStreamer::get_sink_image(int port) {
    if (!appsinks.count(port))
        return nullptr;

    GstElement *appsink = appsinks[port];

    GstSample *sample = gst_app_sink_try_pull_sample(GST_APP_SINK(appsink), 0 * GST_MSECOND);
    if (!sample)
        return nullptr;

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstCaps *caps = gst_sample_get_caps(sample);
    if (!caps) {
        gst_sample_unref(sample);
        return nullptr;
    }
    GstStructure *s = gst_caps_get_structure(caps, 0);

    int width = 0, height = 0;
    gst_structure_get_int(s, "width", &width);
    gst_structure_get_int(s, "height", &height);

    GstMapInfo map;
    if (!gst_buffer_map(buffer, &map, GST_MAP_READ)) {
        gst_sample_unref(sample);
        return nullptr;
    }

    PackedByteArray map_data;
    map_data.resize(width * height * 3);
    memcpy(map_data.ptrw(), map.data, map.size);

    Ref<Image> img = Image::create_from_data(width, height, false, Image::FORMAT_RGB8, map_data);

    images[port] = img;

    gst_buffer_unmap(buffer, &map);
    gst_sample_unref(sample);

    return images[port];
}



void GStreamer::stop_sink_stream(int port) {
    if (!pipelines.count(port))
        return;

    gst_element_set_state(pipelines[port], GST_STATE_NULL);
    gst_object_unref(pipelines[port]);
    gst_object_unref(appsinks[port]);
    pipelines.erase(port);
    appsinks.erase(port);
    images.erase(port);
    UtilityFunctions::print("Пайплайн " + UtilityFunctions::str(port) + " остановлен.");
}