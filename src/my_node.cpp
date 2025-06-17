#include "my_node.hpp"

// #include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

// #include <gst/gst.h>
#include <gst/app/gstappsink.h>


using namespace godot;

// typedef void (*gst_init_func)(int*, char***);

void MyNode::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("hello_node"), &MyNode::hello_node);
	ClassDB::bind_method(D_METHOD("open_test_window"), &MyNode::open_test_window);
    ClassDB::bind_method(D_METHOD("start_stream"), &MyNode::start_stream);
    ClassDB::bind_method(D_METHOD("get_texture"), &MyNode::get_texture);
	ClassDB::bind_method(D_METHOD("stop_stream"), &MyNode::stop_stream);
}

MyNode::MyNode()
{
}

MyNode::~MyNode()
{
}

// Override built-in methods with your own logic. Make sure to declare them in the header as well!

void MyNode::_ready()
{
	godot::print_line("Kek8");

	
}

void MyNode::_process(double delta)
{
}


void MyNode::start_stream() {
    gst_init(nullptr, nullptr);

    // Пайплайн для приёма H264 по UDP и декодирования
    const char *pipeline_desc =
        "udpsrc port=5000 caps=\"application/x-rtp, media=video, encoding-name=H264, payload=96\" ! "
        "rtph264depay ! avdec_h264 ! videoconvert ! video/x-raw,format=RGB ! appsink name=appsink";

    GError *error = nullptr;
    pipeline = gst_parse_launch(pipeline_desc, &error);
    if (!pipeline) {
        UtilityFunctions::print("Failed to create pipeline");
        return;
    }

    appsink = gst_bin_get_by_name(GST_BIN(pipeline), "appsink");
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
}


Ref<ImageTexture> MyNode::get_texture() {
    if (!appsink)
        return nullptr;

    // Ждём кадр до 100 мс
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

    // Записываем байты в map_data
    PackedByteArray map_data;
    map_data.resize(width * height * 3); // RGB8 format, 3 bytes per pixel
    memcpy(map_data.ptrw(), map.data, map.size);

    // Создаём Image из map_data
    // Ref<Image> img = memnew(Image);
    Ref<Image>img = Image::create_from_data(width, height, false, Image::FORMAT_RGB8, map_data);

    // Создаём ImageTexture из Image
    if (!texture.is_valid()) {
        // texture = memnew(ImageTexture);
        // texture.instantiate();
        // Если это первый кадр, создаём новый ImageTexture
        texture = ImageTexture::create_from_image(img);
    } else {
        // Иначе обновляем существующий ImageTexture, что б избежать лишних аллокаций
        texture->update(img);
    }

    gst_buffer_unmap(buffer, &map);
    gst_sample_unref(sample);

    return texture;
}




void MyNode::stop_stream() {
	gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}


void MyNode::open_test_window() {
    gst_init(nullptr, nullptr);

    // Use videotestsrc for a test pattern
    GstElement *test_pipeline = gst_parse_launch(
        "videotestsrc ! videoconvert ! autovideosink", nullptr);

    if (!test_pipeline) {
        g_printerr("Failed to create pipeline.\n");
        return;
    }

    gst_element_set_state(test_pipeline, GST_STATE_PLAYING);

    // Run a GLib main loop to keep the window open
    GMainLoop *loop = g_main_loop_new(nullptr, FALSE);
    g_main_loop_run(loop);

    // Cleanup
    gst_element_set_state(test_pipeline, GST_STATE_NULL);
    gst_object_unref(test_pipeline);
    g_main_loop_unref(loop);
}



godot::String MyNode::hello_node()
{
	return "Hello GDExtension Node\n";
}
