extends CharacterBody3D

const SPEED = 5.0
const JUMP_VELOCITY = 4.5

# Get the gravity from the project settings to be synced with RigidBody nodes.
var gravity = ProjectSettings.get_setting("physics/3d/default_gravity")

@export var current_class: RobotClass
var max_health = 100
var current_health = 100

signal health_changed(current, max)

@onready var nav_agent = $NavigationAgent3D
@onready var camera = $Camera3D
@onready var mesh = $MeshInstance3D

var projectile_scene = preload("res://scenes/projectile.tscn")

func _ready():
	if current_class:
		print("Loaded Class: ", current_class.robot_name)
		# Apply Class Stats
		max_health = current_class.base_health
		current_health = max_health

var hidden_walls = {} # Dictionary of hidden walls

func take_damage(amount):
	current_health -= amount
	health_changed.emit(current_health, max_health)
	print("Player took ", amount, " damage! Health: ", current_health)
	if current_health <= 0:
		die()

func die():
	print("Player Died!")
	# Reload scene for now
	get_tree().reload_current_scene()

func _physics_process(delta):
	# Add gravity
	if not is_on_floor():
		velocity.y -= gravity * delta

	# 1. Handle Movement
	var input_dir = Input.get_vector("move_left", "move_right", "move_forward", "move_backward")
	
	var current_speed = SPEED
	if current_class:
		current_speed = current_class.movement_speed

	if input_dir:
		# WASD Control (Overrides Navigation)
		nav_agent.target_position = global_position # Stop navigation
		var direction = (transform.basis * Vector3(input_dir.x, 0, input_dir.y)).normalized()
		velocity.x = direction.x * current_speed
		velocity.z = direction.z * current_speed
	elif not nav_agent.is_navigation_finished():
		# Navigation Control
		var current_agent_position = global_position
		var next_path_position = nav_agent.get_next_path_position()
		var new_velocity = (next_path_position - current_agent_position).normalized() * current_speed
		velocity.x = new_velocity.x
		velocity.z = new_velocity.z
	else:
		# Idle
		velocity.x = move_toward(velocity.x, 0, current_speed)
		velocity.z = move_toward(velocity.z, 0, current_speed)

	move_and_slide()
	
	# 2. Handle Rotation (Look at Mouse)
	look_at_mouse()
	
	# 3. Handle Wall Occlusion
	check_wall_occlusion()

func check_wall_occlusion():
	var space_state = get_world_3d().direct_space_state
	var origin = camera.global_position
	var target = global_position + Vector3(0, 1, 0)
	var dist = origin.distance_to(target)
	
	# Create a Sphere Shape for the cast
	var shape = SphereShape3D.new()
	shape.radius = 3.5 # Hide walls within 3.5m of the check point
	
	var check_pos = global_position + Vector3(0, 1, 0) + (origin - target).normalized() * 2.0
	
	var query = PhysicsShapeQueryParameters3D.new()
	query.shape = shape
	query.transform = Transform3D(Basis.IDENTITY, check_pos)
	
	# Limit results (Increase to catch all walls)
	var results = space_state.intersect_shape(query, 128)
	
	var currently_hidden = {}
	
	for result in results:
		var collider = result.collider
		if collider.is_in_group("wall"):
			currently_hidden[collider] = true
			if not hidden_walls.has(collider):
				if collider.has_method("fade_out"):
					collider.fade_out()
				hidden_walls[collider] = true
	
	# Restore walls that are no longer in the hit list
	var walls_to_restore = []
	for wall in hidden_walls.keys():
		if not currently_hidden.has(wall):
			if wall.has_method("fade_in"):
				wall.fade_in()
			walls_to_restore.append(wall)
			
	for wall in walls_to_restore:
		hidden_walls.erase(wall)

func _unhandled_input(event):
	if event.is_action_pressed("move_click"):
		set_click_target()
	elif event.is_action_pressed("attack_primary"):
		shoot()
	elif event.is_action_pressed("attack_secondary"):
		use_skill()

func use_skill():
	if current_class and current_class.starting_skill_scene:
		var skill = current_class.starting_skill_scene.instantiate()
		get_parent().add_child(skill)
		skill.global_position = global_position

func shoot():
	var projectile = projectile_scene.instantiate()
	get_parent().add_child(projectile)
	projectile.global_position = global_position + Vector3(0, 1, 0) - mesh.global_transform.basis.z * 1.0
	projectile.global_rotation = mesh.global_rotation

func set_click_target():
	var mouse_pos = get_viewport().get_mouse_position()
	var origin = camera.project_ray_origin(mouse_pos)
	var normal = camera.project_ray_normal(mouse_pos)
	var query = PhysicsRayQueryParameters3D.create(origin, origin + normal * 1000)
	
	var space_state = get_world_3d().direct_space_state
	var result = space_state.intersect_ray(query)
	
	if result:
		nav_agent.target_position = result.position

func look_at_mouse():
	var mouse_pos = get_viewport().get_mouse_position()
	var origin = camera.project_ray_origin(mouse_pos)
	var normal = camera.project_ray_normal(mouse_pos)
	
	# Create a plane at the player's height (y) to intersect with
	var player_plane = Plane(Vector3.UP, global_position.y)
	var intersect_point = player_plane.intersects_ray(origin, normal)
	
	if intersect_point:
		# Look at the point, but keep Y rotation only
		var look_target = Vector3(intersect_point.x, global_position.y, intersect_point.z)
		mesh.look_at(look_target, Vector3.UP)
