extends Control

@onready var grid_container = $Panel/GridContainer
@onready var equip_container = $Panel/EquipContainer
@onready var slot_scene = preload("res://scenes/ui/inventory_slot.tscn")

var inventory: Inventory = null
var equip_slots = {} # Map Slot Enum -> UI Slot Instance

func _ready():
	visible = false
	call_deferred("_init_equipment_slots")
	
	# Find player and connect
	await get_tree().process_frame
	var players = get_tree().get_nodes_in_group("player")
	if players.size() > 0:
		var player = players[0]
		if player.has_node("Inventory"):
			inventory = player.get_node("Inventory")
			inventory.inventory_changed.connect(_on_inventory_changed)
			inventory.equipment_changed.connect(_on_equipment_changed)
			# Initial draw
			_on_inventory_changed(inventory.items)
			_on_equipment_changed(inventory.equipment)

func _init_equipment_slots():
	# Clear existing
	for child in equip_container.get_children():
		child.queue_free()
	
	# Create fixed slots for each Equipment.Slot
	for slot_enum in Equipment.Slot.values():
		var slot_ui = slot_scene.instantiate()
		equip_container.add_child(slot_ui)
		slot_ui.init(null, slot_enum) # Pass Enum as index for callback
		slot_ui.clicked.connect(_on_equip_slot_clicked)
		
		# Set placeholder text/tooltip based on Enum key
		var slot_name = Equipment.Slot.keys()[slot_enum]
		slot_ui.tooltip_text = slot_name
		# Optional: Set a label on the slot if we modify slot_scene, but for now tooltip is fine
		
		equip_slots[slot_enum] = slot_ui

func _unhandled_input(event):
	if event.is_action_pressed("toggle_inventory"):
		visible = !visible
		if visible:
			Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
		else:
			Input.mouse_mode = Input.MOUSE_MODE_CONFINED

func _on_inventory_changed(items: Array):
	# Clear current slots
	for child in grid_container.get_children():
		child.queue_free()
	
	# Create new slots
	for i in range(items.size()):
		var slot = slot_scene.instantiate()
		grid_container.add_child(slot)
		slot.init(items[i], i)
		slot.clicked.connect(_on_slot_clicked)

func _on_equipment_changed(equipment: Dictionary):
	if equip_slots.is_empty():
		_init_equipment_slots()
		
	for slot_enum in equipment.keys():
		var item = equipment[slot_enum]
		var slot_ui = equip_slots.get(slot_enum)
		if slot_ui:
			# Wrap in data object to match init signature {item, amount}
			if item:
				slot_ui.init({"item": item, "amount": 1}, slot_enum)
			else:
				slot_ui.init(null, slot_enum)
				# Restore Tooltip
				slot_ui.tooltip_text = Equipment.Slot.keys()[slot_enum]

func _on_slot_clicked(index: int):
	# Equip from Inventory
	if inventory:
		inventory.equip_item(index)

func _on_equip_slot_clicked(slot_enum: int):
	# Unequip
	if inventory:
		inventory.unequip_item(slot_enum)
