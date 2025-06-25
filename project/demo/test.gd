extends Node

var _should_get_video_from_port := false
var _gstreamer: GStreamer

@onready var _frames := [
	$VBoxContainer/Frames/FramePanel/FrameContainer/Frame,
	$VBoxContainer/Frames/FramePanel2/FrameContainer/Frame,
	$VBoxContainer/Frames/FramePanel3/FrameContainer/Frame,
]


func _ready() -> void:
	_gstreamer = GStreamer.new()
	

func _process(delta: float) -> void:
	if !_should_get_video_from_port: return
	var tex = _gstreamer.get_texture(5000)
	if tex:
		_frames[0].texture = tex
	var tex2 = _gstreamer.get_texture(5001)
	if tex2:
		_frames[1].texture = tex2
	var tex3 = _gstreamer.get_texture(5002)
	if tex3:
		_frames[2].texture = tex3


func _on_get_pressed() -> void:
	_should_get_video_from_port = true
	_gstreamer.start_stream(5000)
	_gstreamer.start_stream(5001)
	_gstreamer.start_stream(5002)
