extends Area3D

const SPEED = 20.0
const DAMAGE = 10

func _physics_process(delta):
	position -= transform.basis.z * SPEED * delta

func _on_body_entered(body):
	if body.name == "Player":
		return
		
	if body.has_method("take_damage"):
		body.take_damage(DAMAGE)
	queue_free()

func _on_timer_timeout():
	queue_free()
