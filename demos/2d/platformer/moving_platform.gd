
extends Node2D

# member variables here, example:
# var a=2
# var b="textvar"

export var motion = Vector2()
export var cycle = 1.0
var accum=0.0

func _fixed_process(delta):

	accum += delta * (1.0/cycle) * PI * 2.0
	accum = fmod(accum,PI*2.0)
	var d = sin(accum)
	var xf = Transform2D()
	xf[2]= motion * d 
	get_node("platform").set_transform(xf)
		

func _ready():
	# Initalization here
	set_fixed_process(true)
	pass


