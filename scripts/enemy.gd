extends CharacterBody3D

var health = 50

func take_damage(amount):
	health -= amount
	print("Enemy took ", amount, " damage! Health: ", health)
	
	# Visual feedback (flash red)
	var mesh = $MeshInstance3D
	var original_material = mesh.get_active_material(0)
	var flash_material = StandardMaterial3D.new()
	flash_material.albedo_color = Color.RED
	mesh.set_surface_override_material(0, flash_material)
	
	await get_tree().create_timer(0.1).timeout
	mesh.set_surface_override_material(0, null)
	
	if health <= 0:
		die()

func die():
	print("Enemy died!")
	queue_free()
