extends Area3D

const SPEED = 20.0
var damage = 10

func _physics_process(delta):
	position -= transform.basis.z * SPEED * delta

var shooter = null

func _on_body_entered(body):
	if body == shooter:
		return
		
	if body.has_method("take_damage"):
		body.take_damage(damage)
	queue_free()

func _on_timer_timeout():
	queue_free()
