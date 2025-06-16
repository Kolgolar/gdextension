extends Node

@onready var label: Label = $Label

func _ready() -> void:
	label.text += "Hello GDScript!\n"
	label.text += $MyNode.hello_node()
	MySingleton.hello_singleton(label)
	


func _on_button_pressed() -> void:
	$MyNode.start_stream("127.0.0.1", 5000)
