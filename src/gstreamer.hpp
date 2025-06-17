#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <gst/gst.h>

using namespace godot;

class GStreamer : public RefCounted
{
	GDCLASS(GStreamer, RefCounted);

protected:
	static void _bind_methods();

private:
    GstElement *pipeline = nullptr;
    GstElement *appsink = nullptr;
	Ref<ImageTexture> texture;

public:

	GStreamer();
	~GStreamer();

	// void _ready() override;
	// void _process(double delta) override;

	Ref<ImageTexture> get_texture();
	void open_test_window();
	void start_stream();
    void stop_stream();
};
