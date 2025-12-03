extends Area3D

const DAMAGE = 25
const DURATION = 0.5

func _ready():
	# Auto-destroy after duration
	await get_tree().create_timer(DURATION).timeout
	queue_free()

func _on_body_entered(body):
	if body.name == "Player":
		return
		
	if body.has_method("take_damage"):
		body.take_damage(DAMAGE)
