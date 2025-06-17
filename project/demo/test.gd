extends Node

var _should_get_video_from_port := false
var _gstreamer: GStreamer

@onready var _frames := [
	$VBoxContainer/Frames/FramePanel/FrameContainer/Frame,
	$VBoxContainer/Frames/FramePanel2/FrameContainer/Frame
]


func _ready() -> void:
	_gstreamer = GStreamer.new()
	#label.text += "Hello GDScript!\n"
	#label.text += $GStreamer.hello_node()
	#MySingleton.hello_singleton(label)
	

func _process(delta: float) -> void:
	if !_should_get_video_from_port: return
	var tex = _gstreamer.get_texture(5000)
	if tex:
		_frames[0].texture = tex
	var tex2 = _gstreamer.get_texture(5001)
	if tex2:
		_frames[1].texture = tex2
	
	#var bytes = $GStreamer.get_texture_bytes()
	#if bytes.size() == 0: return
	#var img = Image.create_from_data(320, 240, false, Image.FORMAT_RGB8, bytes)
	#var tex = ImageTexture.create_from_image(img)
	#if tex:
		#$TextureRect.texture = tex


func _on_get_pressed() -> void:
	_should_get_video_from_port = true
	_gstreamer.start_stream(5000)
	_gstreamer.start_stream(5001)
