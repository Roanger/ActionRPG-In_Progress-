extends CharacterBody3D

const SPEED = 4.0
const ATTACK_RANGE = 2.0
const DAMAGE = 10

var health = 50
var player = null

@onready var nav_agent = $NavigationAgent3D

func _ready():
	# Find player slightly after load to ensure they exist
	await get_tree().process_frame
	var players = get_tree().get_nodes_in_group("player")
	if players.size() > 0:
		player = players[0]

const ATTACK_COOLDOWN = 1.0
var can_attack = true

func _physics_process(delta):
	# Add Gravity
	if not is_on_floor():
		velocity.y -= 9.8 * delta

	if player:
		var dist_to_player = global_position.distance_to(player.global_position)
		
		# 1. Update Path (Only chase if outside attack range)
		if dist_to_player > ATTACK_RANGE:
			nav_agent.target_position = player.global_position
			
			if not nav_agent.is_navigation_finished():
				var next_position = nav_agent.get_next_path_position()
				# Keep Y velocity separate (handled by gravity/jump)
				var current_pos = global_position
				var target_pos = next_position
				target_pos.y = current_pos.y # Ignore Y for direction calculation
				
				var direction = (target_pos - current_pos).normalized()
				velocity.x = direction.x * SPEED
				velocity.z = direction.z * SPEED
				
				# Face player
				look_at(Vector3(player.global_position.x, global_position.y, player.global_position.z), Vector3.UP)
		else:
			# Stop moving when in range
			velocity.x = 0
			velocity.z = 0
			
			# Attack if cooldown ready
			if can_attack:
				attack()

		move_and_slide()

func attack():
	if player.has_method("take_damage"):
		player.take_damage(DAMAGE)
		can_attack = false
		
		# Visual feedback (Jump) -> DISABLED to prevent bouncy physics
		# velocity.y = 5.0 
		
		await get_tree().create_timer(ATTACK_COOLDOWN).timeout
		can_attack = true

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
