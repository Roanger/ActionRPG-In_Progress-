class_name WorldItem
extends Area3D

@export var item: Item
@export var amount: int = 1

@onready var sprite = $Sprite3D
@onready var label = $Label3D

func _ready():
	body_entered.connect(_on_body_entered)
	setup_visuals()

func setup_visuals():
	if item:
		if item.icon:
			sprite.texture = item.icon
		label.text = item.name + (" x" + str(amount) if amount > 1 else "")

func _on_body_entered(body):
	if body.is_in_group("player"):
		# Try to add to inventory
		if body.has_node("Inventory"):
			var inventory = body.get_node("Inventory")
			var leftover = inventory.add_item(item, amount)
			
			if leftover == 0:
				# Fully picked up
				queue_free()
			else:
				# Inventory full, update amount
				amount = leftover
				setup_visuals()
				print("Inventory full!")

func set_item(new_item: Item, new_amount: int):
	item = new_item
	amount = new_amount
	setup_visuals()
