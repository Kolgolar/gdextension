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
	GstElement *pipeline;
    Ref<ImageTexture> texture;

public:
	MyNode();
	~MyNode();

	void _ready() override;
	void _process(double delta) override;

	// void get_version();
	void start_stream(String host, int port);
    void stop_stream();
	Ref<ImageTexture> get_texture() { return texture; }

	godot::String hello_node();
};
