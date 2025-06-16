extends Node

@onready var label: Label = $Label
var _should_get_video_from_port := false


func _ready() -> void:
	label.text += "Hello GDScript!\n"
	label.text += $MyNode.hello_node()
	MySingleton.hello_singleton(label)
	

func _process(delta: float) -> void:
	if !_should_get_video_from_port: return
	var tex = $MyNode.get_texture()
	if tex:
		$TextureRect.texture = tex


func _on_test_pressed() -> void:
	$MyNode.open_test_window()


func _on_get_pressed() -> void:
	_should_get_video_from_port = true
	$MyNode.start_stream()
