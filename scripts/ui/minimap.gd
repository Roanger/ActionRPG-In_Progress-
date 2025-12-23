extends Control

@onready var sub_viewport = $SubViewportContainer/SubViewport
@onready var camera = $SubViewportContainer/SubViewport/Camera3D
@onready var objectives_label = $ObjectivesPanel/Label

var player: Node3D = null

func _ready():
	# Allow the Viewport to see the current 3D world
	# In Godot 4, SubViewports don't automatically share the world of the parent if they are in a separate CanvasLayer/Control context sometimes.
	# But typically explicit World3D assignment or just being in the tree works.
	# We might need to ensuring the camera culls correctly.
	# Find player
	await get_tree().process_frame
	
	# Explicitly share the main 3D world with the minimap viewport
	if get_tree().current_scene:
		sub_viewport.world_3d = get_tree().current_scene.get_world_3d()
	
	var players = get_tree().get_nodes_in_group("player")
	if players.size() > 0:
		player = players[0]

func _process(delta):
	if player:
		# Follow player X/Z
		camera.global_position.x = player.global_position.x
		camera.global_position.z = player.global_position.z
		# Keep Y high
		camera.global_position.y = player.global_position.y + 20.0
		
		# Optional: Rotate with player?
		# camera.global_rotation.y = player.global_rotation.y
