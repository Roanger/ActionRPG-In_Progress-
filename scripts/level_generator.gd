extends Node3D

@export var floor_scene: PackedScene
@export var wall_scene: PackedScene
@export var grid_size: int = 2
@export var max_steps: int = 1000
@export var player_scene: PackedScene
@export var enemy_scene: PackedScene

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
	
	# 2. Place Floors
	for pos in _visited.keys():
		_place_tile(floor_scene, pos)
	
	# 3. Place Walls (surrounding floors)
	var wall_positions = {}
	for pos in _visited.keys():
		for dir in [Vector2.UP, Vector2.DOWN, Vector2.LEFT, Vector2.RIGHT]:
			var neighbor = pos + dir
			if not _visited.has(neighbor) and not wall_positions.has(neighbor):
				_place_tile(wall_scene, neighbor)
				wall_positions[neighbor] = true

	# 4. Bake Navigation
	_bake_navmesh()
	
	# 5. Spawn Entities
	spawn_entities()

func spawn_entities():
	var valid_positions = _visited.keys()
	
	if valid_positions.size() > 0:
		# Spawn Player at Start (0,0 is always in visited)
		var start_pos = Vector2.ZERO
		if player_scene:
			var player = player_scene.instantiate()
			get_parent().get_parent().add_child(player) # Add to Main scene root
			player.global_position = Vector3(start_pos.x * grid_size, 1, start_pos.y * grid_size)
		
		# Spawn Enemy at Random Valid Position (away from start)
		if enemy_scene:
			var enemy_pos = valid_positions.pick_random()
			# Simple check to avoid spawning on top of player (if map is tiny)
			while enemy_pos == start_pos and valid_positions.size() > 1:
				enemy_pos = valid_positions.pick_random()
				
			var enemy = enemy_scene.instantiate()
			get_parent().get_parent().add_child(enemy)
			enemy.global_position = Vector3(enemy_pos.x * grid_size, 1, enemy_pos.y * grid_size)
			print("Spawned Enemy at: ", enemy.global_position)
		
		print("Spawned Player at: ", Vector3(start_pos.x * grid_size, 1, start_pos.y * grid_size))

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
