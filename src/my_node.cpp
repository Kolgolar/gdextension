#include "my_node.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <iostream>

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <windows.h>

#include <godot_cpp/classes/video_stream.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// typedef void (*gst_init_func)(int*, char***);

void MyNode::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("hello_node"), &MyNode::hello_node);
	ClassDB::bind_method(D_METHOD("start_stream"), &MyNode::start_stream);
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


void MyNode::start_stream(String host, int port) {
    // Инициализация GStreamer
    gst_init(nullptr, nullptr);

    // Создание pipeline
	gst_debug_set_default_threshold(GST_LEVEL_DEBUG);
    GstElement* pipeline = gst_pipeline_new("webcam-pipeline");
    GstElement* source = gst_element_factory_make("ksvideosrc ", "webcam-source"); // Windows использует ksvideosrc
    GstElement* convert = gst_element_factory_make("videoconvert", "converter");
    GstElement* sink = gst_element_factory_make("autovideosink", "video-output");

    if (!pipeline || !source || !convert || !sink) {
        std::cerr << "Error: Can't create GStreamer elements!" << std::endl;
        return;
    }

    g_object_set(source, "device-index", 0, NULL);

    // Добавление элементов в pipeline
    gst_bin_add_many(GST_BIN(pipeline), source, convert, sink, NULL);

    // Соединение элементов: source → convert → sink
    if (!gst_element_link_many(source, convert, sink, NULL)) {
        std::cerr << "Error: Can't connect the elements!" << std::endl;
        gst_object_unref(pipeline);
        return;
    }

    // Запуск pipeline
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        std::cerr << "Error: Can't launch the pipeline!" << std::endl;
        gst_object_unref(pipeline);
        return;
    }

    std::cout << "Webcam is on. Press Enter to end this madness..." << std::endl;
    std::cin.get(); // Ждём нажатия Enter
}

void MyNode::stop_stream() {
	gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}


// void get_version() {

// }


godot::String MyNode::hello_node()
{
	return "Hello GDExtension Node\n";
}
