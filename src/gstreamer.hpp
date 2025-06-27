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
    std::map<int, GstElement*> pipelines; // Все пайплайны
    std::map<int, GstElement*> appsinks; // Все потоки, принимающие кадры
    std::map<int, Ref<Image>> images; // Все полученные кадры (в виде Годотовского Image)

public:
    GStreamer();
    ~GStreamer();
    // Обращаемся к созданному через пайплайн потоку,
    // чтобы получить последний актуальный кадр
    Ref<Image> get_sink_image(int port);
    // Передаём пайплайн и создаём поток, который будет принимать кадры
    void start_sink_stream(godot::String pipeline_desc);
    // Останавливаем поток приёма кадров
    void stop_sink_stream(int port);
};
