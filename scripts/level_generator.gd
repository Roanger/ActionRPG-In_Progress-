extends Node3D

@export var floor_scene: PackedScene
@export var wall_scene: PackedScene
@export var grid_size: int = 4
@export var max_steps: int = 1000
@export var player_scene: PackedScene
@export var enemy_scene: PackedScene
@export var ranged_enemy_scene: PackedScene
@export var boss_scene: PackedScene
@export var arena_size: int = 4

var _visited = {}


func _ready():
	await get_tree().process_frame
	generate_level()

func generate_level():
	print("Generating Level...")
	var current_pos = Vector2.ZERO
	_visited[current_pos] = true
	
	for i in range(max_steps):
		var direction = _get_random_direction()
		current_pos += direction
		
		# Stamp a 2x2 area to ensure minimum 2x2 width everywhere
		_visited[current_pos] = true
		_visited[current_pos + Vector2(1, 0)] = true
		_visited[current_pos + Vector2(0, 1)] = true
		_visited[current_pos + Vector2(1, 1)] = true
	
	var boss_arena_center = current_pos
	_generate_arena(boss_arena_center)
	
	# 1. Place Floors
	for pos in _visited.keys():
		_place_tile(floor_scene, pos)
	
	# 2. Place Walls at the edges of visited cells
	for pos in _visited.keys():
		for dir in [Vector2.UP, Vector2.DOWN, Vector2.LEFT, Vector2.RIGHT]:
			var neighbor = pos + dir
			if not _visited.has(neighbor):
				_place_wall_at_edge(pos, dir)

	_bake_navmesh()
	spawn_entities(boss_arena_center)

func _generate_arena(center):
	var radius = arena_size
	for x in range(-radius, radius + 1):
		for y in range(-radius, radius + 1):
			_visited[center + Vector2(x, y)] = true

func spawn_entities(boss_pos = Vector2.ZERO):
	var valid_positions = _visited.keys()
	
	if valid_positions.size() > 0:
		var start_pos = Vector2.ZERO
		if player_scene:
			var player = player_scene.instantiate()
			get_parent().get_parent().add_child(player)
			player.global_position = Vector3(start_pos.x * grid_size, 1, start_pos.y * grid_size)
		
		if boss_scene:
			var boss = boss_scene.instantiate()
			get_parent().get_parent().add_child(boss)
			boss.global_position = Vector3(boss_pos.x * grid_size, 1, boss_pos.y * grid_size)
			boss.scale = Vector3(2, 2, 2)
		
		if enemy_scene or ranged_enemy_scene:
			for i in range(10):
				var enemy_pos = valid_positions.pick_random()
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
		# Floor scale is already set inside the floor_tile.tscn model transform
		if scene == floor_scene:
			tile.scale = Vector3(1.0, 1.0, 1.0)

func _place_wall_at_edge(floor_pos, direction):
	if wall_scene:
		var wall = wall_scene.instantiate()
		add_child(wall)
		
		# Center of current floor
		var base_pos = Vector3(floor_pos.x * grid_size, 0, floor_pos.y * grid_size)
		# Move to edge (half of 4m grid = 2m)
		var offset = Vector3(direction.x, 0, direction.y) * (grid_size / 2.0)
		wall.position = base_pos + offset
		
		# Rotate to face the room center
		if direction == Vector2.UP: # North edge, face South
			wall.rotation_degrees.y = 180
		elif direction == Vector2.DOWN: # South edge, face North
			wall.rotation_degrees.y = 0
		elif direction == Vector2.LEFT: # West edge, face East
			wall.rotation_degrees.y = -90
		elif direction == Vector2.RIGHT: # East edge, face West
			wall.rotation_degrees.y = 90

func _bake_navmesh():
	var nav_region = get_parent()
	if nav_region is NavigationRegion3D:
		nav_region.bake_navigation_mesh(true)
