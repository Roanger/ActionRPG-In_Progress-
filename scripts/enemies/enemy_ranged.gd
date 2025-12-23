extends EnemyBase

@export var projectile_scene: PackedScene
@export var flee_distance: float = 4.0

func _process_ai(delta):
	var dist_to_player = global_position.distance_to(player.global_position)
	
	# Face player always
	look_at_player()
	
	if dist_to_player < flee_distance:
		# Too close! Flee away from player
		var flee_dir = (global_position - player.global_position).normalized()
		var flee_target = global_position + flee_dir * 5.0
		move_towards_target(flee_target)
		
	elif dist_to_player > attack_range:
		# Too far! Chase to get in range
		move_towards_target(player.global_position)
		
	else:
		# In Sweet Spot: Stop and Shoot
		velocity.x = 0
		velocity.z = 0
		if can_attack:
			force_attack()

func force_attack():
	can_attack = false
	shoot_projectile()
	await get_tree().create_timer(attack_cooldown).timeout
	can_attack = true

func shoot_projectile():
	if projectile_scene:
		var proj = projectile_scene.instantiate()
		get_parent().add_child(proj)
		
		# Position at "head" height
		var spawn_pos = global_position + Vector3(0, 1.0, 0) + transform.basis.z * 1.0
		proj.global_position = spawn_pos
		proj.global_rotation = global_rotation
		
		# Set shooter key
		if "shooter" in proj:
			proj.shooter = self
