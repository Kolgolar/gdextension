#include "gstreamer.hpp"

// #include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

#include <gst/app/gstappsink.h>


using namespace godot;

// typedef void (*gst_init_func)(int*, char***);

void GStreamer::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("start_stream"), &GStreamer::start_stream);
    ClassDB::bind_method(D_METHOD("get_texture"), &GStreamer::get_texture);
	ClassDB::bind_method(D_METHOD("stop_stream"), &GStreamer::stop_stream);
}

GStreamer::GStreamer()
{
    UtilityFunctions::print("GStreamer addon is ready!");
}

GStreamer::~GStreamer()
{
}

// Override built-in methods with your own logic. Make sure to declare them in the header as well!

// void GStreamer::_ready()
// {
// }

// void GStreamer::_process(double delta)
// {
// }


void GStreamer::start_stream(int port) {
    gst_init(nullptr, nullptr);

    if (pipelines.count(port)) {
        UtilityFunctions::print("Pipeline for this port already exists.");
        return;
    }

    std::string pipeline_desc =
        "udpsrc port=" + std::to_string(port) + " caps=\"application/x-rtp, media=video, encoding-name=H264, payload=96\" ! "
        "rtph264depay ! avdec_h264 ! videoconvert ! video/x-raw,format=RGB ! appsink name=appsink";

    GError *error = nullptr;
    GstElement *pipeline = gst_parse_launch(pipeline_desc.c_str(), &error);
    if (!pipeline) {
        UtilityFunctions::print("Failed to create pipeline");
        return;
    }

    GstElement *appsink = gst_bin_get_by_name(GST_BIN(pipeline), "appsink");
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    pipelines[port] = pipeline;
    appsinks[port] = appsink;
}

// gst-launch-1.0 videotestsrc ! video/x-raw, framerate=30/1, width=1920, height=1080 ! videoconvert ! x264enc tune=zerolatency ! rtph264pay ! udpsink host=127.0.0.1 port=5000

Ref<ImageTexture> GStreamer::get_texture(int port) {
    if (!appsinks.count(port))
        return nullptr;

    GstElement *appsink = appsinks[port];

    GstSample *sample = gst_app_sink_try_pull_sample(GST_APP_SINK(appsink), 100 * GST_MSECOND);
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

    if (!textures[port].is_valid()) {
        textures[port] = ImageTexture::create_from_image(img);
    } else {
        textures[port]->update(img);
    }

    gst_buffer_unmap(buffer, &map);
    gst_sample_unref(sample);

    return textures[port];
}

void GStreamer::stop_stream(int port) {
    if (!pipelines.count(port))
        return;

    gst_element_set_state(pipelines[port], GST_STATE_NULL);
    gst_object_unref(pipelines[port]);
    gst_object_unref(appsinks[port]);
    pipelines.erase(port);
    appsinks.erase(port);
    textures.erase(port);
}