extends StaticBody3D

@onready var mesh_instance = $MeshInstance3D
var is_transparent = false

func _ready():
	# Ensure material is unique so we don't fade ALL walls
	var mat = mesh_instance.get_active_material(0)
	if mat:
		mesh_instance.set_surface_override_material(0, mat.duplicate())

func fade_out():
	if is_transparent: return
	is_transparent = true
	var mat = mesh_instance.get_active_material(0)
	if mat:
		# StandardMaterial3D transparency mode
		mat.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
		mat.albedo_color.a = 0.2

func fade_in():
	if not is_transparent: return
	is_transparent = false
	var mat = mesh_instance.get_active_material(0)
	if mat:
		mat.transparency = BaseMaterial3D.TRANSPARENCY_DISABLED
		mat.albedo_color.a = 1.0
