extends Node

@export var _max_framerate := 24.0

var _should_get_video_from_port := false
var _gstreamer: GStreamer
var _img_buffer := [null, null, null]
var _get_texture_thread: Thread
var _mutex: Mutex
var _should_update_frames := false

var _pipeline_base: String = \
"""
udpsrc port=%s ! \
application/x-rtp, media=video, encoding-name=H264, payload=96, clock-rate=90000 ! \
rtpjitterbuffer latency=100 ! rtph264depay ! h264parse ! nvh264dec ! \
videoconvert ! video/x-raw,format=RGB ! \
appsink name=appsink emit-signals=true sync=false max-buffers=1 drop=true
"""

@onready var _frames_textures := [
	$VBoxContainer/Frames/FramePanel/FrameContainer/Frame,
	$VBoxContainer/Frames/FramePanel2/FrameContainer/Frame,
	$VBoxContainer/Frames/FramePanel3/FrameContainer/Frame,
]

func _exit_tree() -> void:
	_stop_process_frames_thread()


func _ready() -> void:
	_gstreamer = GStreamer.new()
	_mutex = Mutex.new()
	

func _process(delta: float) -> void:
	_mutex.lock()
	for i in _frames_textures.size():
		if _img_buffer[i]:
			if _frames_textures[i].texture:
				_frames_textures[i].texture.update(_img_buffer[i])
			else: 
				_frames_textures[i].texture = ImageTexture.create_from_image(_img_buffer[i])
			_img_buffer[i] = null
	_mutex.unlock()
	

func _stop_process_frames_thread():
	$UpdateFrames.stop()
	_should_get_video_from_port = false
	if _get_texture_thread:
		_get_texture_thread.wait_to_finish()
	_should_update_frames = false
		

func _process_frames():
	while(true):
		if !_should_get_video_from_port: break
		if !_should_update_frames: continue
		
		var tex = _gstreamer.get_image(5000)
		if tex:
			_mutex.lock()
			_img_buffer[0] = tex
			_mutex.unlock()
			
		var tex2 = _gstreamer.get_image(5001)
		if tex2:
			_mutex.lock()
			_img_buffer[1] = tex2
			_mutex.unlock()
			
		var tex3 = _gstreamer.get_image(5002)
		if tex3:
			_mutex.lock()
			_img_buffer[2] = tex3
			_mutex.unlock()
		
		_should_update_frames = false


func _on_get_pressed() -> void:
	_gstreamer.start_stream(_pipeline_base % 5000)
	_gstreamer.start_stream(_pipeline_base % 5001)
	_gstreamer.start_stream(_pipeline_base % 5002)
	_should_get_video_from_port = true
	_get_texture_thread = Thread.new()
	_get_texture_thread.start(_process_frames)
	$UpdateFrames.start(1.0 / _max_framerate)


func _on_stop_pressed() -> void:
	_stop_process_frames_thread()


func _on_update_frames_timeout() -> void:
	_should_update_frames = true
