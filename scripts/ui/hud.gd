extends CanvasLayer

@onready var health_bar = $Control/HealthBar
@onready var health_label = $Control/HealthBar/Label

func _ready():
	# Wait for player to exist (Player now spawns dynamically)
	var player = null
	while player == null:
		await get_tree().create_timer(0.2).timeout
		var players = get_tree().get_nodes_in_group("player")
		if players.size() > 0:
			player = players[0]
	
	# Connect to player signal
	player.health_changed.connect(update_health)
	
	# Initialize
	update_health(player.current_health, player.max_health)

func update_health(current, max_health):
	health_bar.max_value = max_health
	health_bar.value = current
	health_label.text = str(current) + " / " + str(max_health)
