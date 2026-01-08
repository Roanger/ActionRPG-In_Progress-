extends StaticBody3D

var meshes: Array = []
var materials: Array = []
var is_transparent = false

func _ready():
	# Find all mesh instances in children (like the new lab models)
	meshes = find_children("*", "MeshInstance3D", true)
	
	for m in meshes:
		# Check all surfaces
		for i in range(m.get_surface_override_material_count()):
			var mat = m.get_active_material(i)
			if mat:
				# Duplicate to avoid fading all walls at once
				var new_mat = mat.duplicate()
				m.set_surface_override_material(i, new_mat)
				materials.append(new_mat)

func fade_out():
	if is_transparent: return
	is_transparent = true
	for mat in materials:
		if mat is BaseMaterial3D:
			mat.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
			mat.albedo_color.a = 0.2

func fade_in():
	if not is_transparent: return
	is_transparent = false
	for mat in materials:
		if mat is BaseMaterial3D:
			mat.transparency = BaseMaterial3D.TRANSPARENCY_DISABLED
			mat.albedo_color.a = 1.0
