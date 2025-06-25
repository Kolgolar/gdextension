#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <gst/gst.h>
#include <map>

using namespace godot;

class GStreamer : public RefCounted
{
    GDCLASS(GStreamer, RefCounted);

protected:
    static void _bind_methods();

private:
    std::map<int, GstElement*> pipelines;
    std::map<int, GstElement*> appsinks;
    std::map<int, Ref<ImageTexture>> textures;

public:
    GStreamer();
    ~GStreamer();

    Ref<ImageTexture> get_texture(int port);
    void start_stream(godot::String pipeline_desc);
    void stop_stream(int port);
};
