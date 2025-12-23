class_name EnemyBase
extends CharacterBody3D

# --- STATS ---
@export var max_health: int = 50
@export var speed: float = 4.0
@export var damage: int = 10
@export var attack_range: float = 2.0
@export var attack_cooldown: float = 1.0

# --- STATE ---
var current_health: int
var player: Node3D = null
var can_attack: bool = true

# --- COMPONENTS ---
@onready var nav_agent: NavigationAgent3D = $NavigationAgent3D
# Optional: Mesh for effects
@onready var mesh: MeshInstance3D = $MeshInstance3D

func _ready():
	current_health = max_health
	# Defer player finding to ensure scene is ready
	await get_tree().process_frame
	find_player()
	
	# TEMP: Force add scrap metal if table is empty
	if loot_table.is_empty():
		var scrap = load("res://resources/items/scrap_metal.tres")
		if scrap:
			loot_table.append(scrap)
			print("Added Scrap Metal to ", name, " loot table (Fallback)")
			
		var blaster = load("res://resources/equipment/rusty_blaster.tres")
		if blaster:
			loot_table.append(blaster)
			
		var plating = load("res://resources/equipment/basic_plating.tres")
		if plating:
			loot_table.append(plating)

func find_player():
	var players = get_tree().get_nodes_in_group("player")
	if players.size() > 0:
		player = players[0]

func _physics_process(delta):
	# Apply Gravity
	if not is_on_floor():
		velocity.y -= 9.8 * delta
	
	if player:
		_process_ai(delta)
	
	move_and_slide()

# --- VIRTUAL METHODS (Override these!) ---
func _process_ai(delta):
	pass

func _perform_attack():
	if player.has_method("take_damage"):
		player.take_damage(damage)

# --- COMMON FUNCTIONS ---
func move_towards_target(target_pos: Vector3):
	nav_agent.target_position = target_pos
	
	if not nav_agent.is_navigation_finished():
		var next_pos = nav_agent.get_next_path_position()
		
		# Flatten Y for direction
		var current_flat = Vector3(global_position.x, 0, global_position.z)
		var next_flat = Vector3(next_pos.x, 0, next_pos.z)
		
		var direction = (next_flat - current_flat).normalized()
		velocity.x = direction.x * speed
		velocity.z = direction.z * speed
		
		# Rotate to face movement (only if moving)
		if direction.length_squared() > 0.001:
			var look_target = Vector3(next_pos.x, global_position.y, next_pos.z)
			if global_position.distance_squared_to(look_target) > 0.001:
				look_at(look_target, Vector3.UP)
	else:
		velocity.x = 0
		velocity.z = 0

func look_at_player():
	if player:
		var look_target = Vector3(player.global_position.x, global_position.y, player.global_position.z)
		look_at(look_target, Vector3.UP)

func take_damage(amount):
	current_health -= amount
	# print(name, " took ", amount, " damage! HP: ", current_health)
	_flash_red()
	
	if current_health <= 0:
		die()

func _flash_red():
	if mesh:
		var flash_mat = StandardMaterial3D.new()
		flash_mat.albedo_color = Color.RED
		mesh.set_surface_override_material(0, flash_mat)
		await get_tree().create_timer(0.1).timeout
		mesh.set_surface_override_material(0, null)

@export var loot_table: Array[Item] = []
@export var loot_chance: float = 1.0

func die():
	print(name, " died!")
	_drop_loot()
	queue_free()

func _drop_loot():
	if loot_table.size() > 0 and randf() < loot_chance:
		var item_to_drop = loot_table.pick_random()
		var world_item = load("res://scenes/systems/world_item.tscn").instantiate()
		# Use main scene as parent instead of self's parent (just to be safe)
		# Use main scene as parent instead of self's parent (just to be safe)
		get_tree().current_scene.add_child(world_item)
		
		# Scatter the drop slightly
		var offset = Vector3(randf_range(-1.0, 1.0), 0.5, randf_range(-1.0, 1.0))
		world_item.global_position = global_position + offset
		
		world_item.set_item(item_to_drop, 1)
		print("Spawned loot: ", item_to_drop.name, " at ", world_item.global_position)
