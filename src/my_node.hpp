#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <gst/gst.h>

using namespace godot;

class MyNode : public Node
{
	GDCLASS(MyNode, Node);

protected:
	static void _bind_methods();
	static GstFlowReturn on_new_sample(GstElement *sink, gpointer user_data);

private:
    GstElement *pipeline = nullptr;
    GstElement *appsink = nullptr;
	Ref<ImageTexture> texture;

public:

	MyNode();
	~MyNode();

	void _ready() override;
	void _process(double delta) override;

	// void get_version();
	Ref<ImageTexture> get_texture();
	PackedByteArray get_texture_bytes();
	void open_test_window();
	void start_stream();
    void stop_stream();

	godot::String hello_node();
};
