extends Control

# Частота, с которой происходит обращение к принимающему пайплайну
@export var _max_framerate := 24.0
# Поток приёма кадров активен, пока эта переменная true
var _keep_process_frames_thread_active := false
# Нужно ли получить новые кадры от gstreamer (контроллируется _max_framerate)
var _should_get_new_frames := false
# Объект, работающий с gstreamer
var _gstreamer: GStreamer
# Буфер кадров, принятых из GDExtension
var _img_buffer: Array[Image]= [null, null, null]
# Поток приёма кадров
var _get_img_thread: Thread
var _mutex: Mutex
# Пайплайн gstreamer для приёма кадров 
var _pipeline_sink: String = \
"""
udpsrc port=%s ! \
application/x-rtp, media=video, encoding-name=H264, payload=96, clock-rate=90000 ! \
rtpjitterbuffer latency=100 ! rtph264depay ! h264parse ! nvh264dec ! \
videoconvert ! video/x-raw,format=RGB ! \
appsink name=appsink emit-signals=true sync=false max-buffers=1 drop=true
"""
# Куда будут выводиться текстуры кадров
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
			# Преобразуем Image в  ImageTexture и выводим на экран
			if _frames_textures[i].texture:
				_frames_textures[i].texture.update(_img_buffer[i])
			else: 
				_frames_textures[i].texture = ImageTexture.create_from_image(_img_buffer[i])
			_img_buffer[i] = null
	_mutex.unlock()
	

func _stop_process_frames_thread():
	# Останавливаем таймер обновления кадров
	$UpdateFrames.stop()
	_should_get_new_frames = false
	# Останавливаем поток приёма кадров
	_keep_process_frames_thread_active = false
	if _get_img_thread:
		_get_img_thread.wait_to_finish()
	# Останавливаем пайплайны gstreamer'a
	_gstreamer.stop_sink_stream(5000)
	_gstreamer.stop_sink_stream(5001)
	_gstreamer.stop_sink_stream(5002)
		
		
# Поток, запрашивающий кадры из gstreamer
func _process_frames():
	while(true):
		if !_keep_process_frames_thread_active: break
		if !_should_get_new_frames: continue
		
		var tex = _gstreamer.get_sink_image(5000)
		if tex:
			_mutex.lock()
			_img_buffer[0] = tex
			_mutex.unlock()
			
		var tex2 = _gstreamer.get_sink_image(5001)
		if tex2:
			_mutex.lock()
			_img_buffer[1] = tex2
			_mutex.unlock()
			
		var tex3 = _gstreamer.get_sink_image(5002)
		if tex3:
			_mutex.lock()
			_img_buffer[2] = tex3
			_mutex.unlock()
		
		_should_get_new_frames = false

# Запускаем пайплайны gstreamer'a
func _on_get_pressed() -> void:
	_gstreamer.start_sink_stream(_pipeline_sink % 5000)
	_gstreamer.start_sink_stream(_pipeline_sink % 5001)
	_gstreamer.start_sink_stream(_pipeline_sink % 5002)
	_keep_process_frames_thread_active = true
	_get_img_thread = Thread.new()
	_get_img_thread.start(_process_frames)
	$UpdateFrames.start(1.0 / _max_framerate)

# Останавливаем пайплайны
func _on_stop_pressed() -> void:
	_stop_process_frames_thread()

# Сообщаем, что необходимо получить новые кадры от gstreamer
func _on_update_frames_timeout() -> void:
	_should_get_new_frames = true
