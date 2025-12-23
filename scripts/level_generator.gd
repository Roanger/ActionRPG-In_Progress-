extends Node3D

@export var floor_scene: PackedScene
@export var wall_scene: PackedScene
@export var grid_size: int = 4
@export var max_steps: int = 1000
@export var player_scene: PackedScene
@export var enemy_scene: PackedScene
@export var ranged_enemy_scene: PackedScene
@export var boss_scene: PackedScene
@export var arena_size: int = 4 # 4x4 radius (so 9x9 roughly?) No, lets do radius.

var _visited = {} # Dictionary acting as Set for Vector2


func _ready():
	# Wait for everything to be ready
	await get_tree().process_frame
	generate_level()

func generate_level():
	print("Generating Level...")
	# 1. Walker Algorithm
	var current_pos = Vector2.ZERO
	_visited[current_pos] = true
	
	for i in range(max_steps):
		var direction = _get_random_direction()
		current_pos += direction
		_visited[current_pos] = true
	
	# 2. Generate BOSS ARENA at the end
	var boss_arena_center = current_pos
	_generate_arena(boss_arena_center)
	
	# 3. Place Floors
	for pos in _visited.keys():
		_place_tile(floor_scene, pos)
	
	# 4. Place Walls (surrounding floors)
	var wall_positions = {}
	for pos in _visited.keys():
		for dir in [Vector2.UP, Vector2.DOWN, Vector2.LEFT, Vector2.RIGHT]:
			var neighbor = pos + dir
			if not _visited.has(neighbor) and not wall_positions.has(neighbor):
				_place_tile(wall_scene, neighbor)
				wall_positions[neighbor] = true

	# 5. Bake Navigation
	_bake_navmesh()
	
	# 6. Spawn Entities (Pass boss center)
	spawn_entities(boss_arena_center)

func _generate_arena(center):
	var radius = arena_size
	for x in range(-radius, radius + 1):
		for y in range(-radius, radius + 1):
			_visited[center + Vector2(x, y)] = true

func spawn_entities(boss_pos = Vector2.ZERO):
	var valid_positions = _visited.keys()
	
	if valid_positions.size() > 0:
		# Spawn Player at Start (0,0 is always in visited)
		var start_pos = Vector2.ZERO
		if player_scene:
			var player = player_scene.instantiate()
			get_parent().get_parent().add_child(player) # Add to Main scene root
			player.global_position = Vector3(start_pos.x * grid_size, 1, start_pos.y * grid_size)
			print("Spawned Player at: ", player.global_position)
		
		# Spawn Boss in Arena Center
		if boss_scene:
			var boss = boss_scene.instantiate()
			get_parent().get_parent().add_child(boss)
			boss.global_position = Vector3(boss_pos.x * grid_size, 1, boss_pos.y * grid_size)
			print("Spawned BOSS at: ", boss.global_position)
			
			# Scale Boss up for fun? (Optional)
			boss.scale = Vector3(2, 2, 2)
		
		# Spawn Random Enemy (Roaming)
		if enemy_scene or ranged_enemy_scene:
			# Spawn 10 random enemies?
			for i in range(10):
				var enemy_pos = valid_positions.pick_random()
				# Avoid Start and Boss Area (roughly)
				if enemy_pos.distance_to(Vector2.ZERO) > 5 and enemy_pos.distance_to(boss_pos) > arena_size + 2:
					var chosen_scene = enemy_scene
					if ranged_enemy_scene and randf() > 0.5:
						chosen_scene = ranged_enemy_scene
					
					if chosen_scene:
						var enemy = chosen_scene.instantiate()
						get_parent().get_parent().add_child(enemy)
						enemy.global_position = Vector3(enemy_pos.x * grid_size, 1, enemy_pos.y * grid_size)

func _get_random_direction():
	var dirs = [Vector2.UP, Vector2.DOWN, Vector2.LEFT, Vector2.RIGHT]
	return dirs.pick_random()

func _place_tile(scene, grid_pos):
	if scene:
		var tile = scene.instantiate()
		add_child(tile)
		tile.position = Vector3(grid_pos.x * grid_size, 0, grid_pos.y * grid_size)

func _bake_navmesh():
	# Find parent NavigationRegion3D and bake
	var nav_region = get_parent()
	if nav_region is NavigationRegion3D:
		print("Baking Navigation Mesh...")
		nav_region.bake_navigation_mesh(true) # true = on_thread
	else:
		printerr("Parent is not NavigationRegion3D!")
