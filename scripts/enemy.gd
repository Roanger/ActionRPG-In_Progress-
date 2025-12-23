extends EnemyBase

func _process_ai(delta):
	var dist_to_player = global_position.distance_to(player.global_position)
	
	if dist_to_player > attack_range:
		move_towards_target(player.global_position)
	else:
		# Attack
		velocity.x = 0
		velocity.z = 0
		look_at_player()
		
		if can_attack:
			force_attack()

func force_attack():
	can_attack = false
	_perform_attack() # Call base damage logic
	
	# Optional: Visual jump (disabled for stability)
	# velocity.y = 5.0
	
	await get_tree().create_timer(attack_cooldown).timeout
	can_attack = true
