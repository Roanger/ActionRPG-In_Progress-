class_name Inventory
extends Node

signal inventory_changed(items)
signal equipment_changed(equipment)
signal item_added(item, amount)
signal item_removed(item, amount)

@export var max_slots: int = 60

# Array of { "item": ItemResource, "amount": int } or null
var items: Array = []
# Dictionary mapping Slot(Enum) -> EquipmentResource
var equipment: Dictionary = {}

func _ready():
	# Initialize empty inventory
	items.resize(max_slots)
	items.fill(null)
	
	# Initialize empty equipment slots
	for slot in Equipment.Slot.values():
		equipment[slot] = null

# --- INVENTORY OPS ---
func add_item(item: Item, amount: int = 1) -> int:
	# 1. Try to stack
	for i in range(max_slots):
		if items[i] and items[i].item.id == item.id and items[i].amount < item.max_stack:
			var space = item.max_stack - items[i].amount
			var to_add = min(amount, space)
			items[i].amount += to_add
			amount -= to_add
			item_added.emit(item, to_add)
			if amount == 0:
				inventory_changed.emit(items)
				return 0
	
	# 2. Try to find empty slot
	if amount > 0:
		for i in range(max_slots):
			if items[i] == null:
				items[i] = {"item": item, "amount": amount}
				item_added.emit(item, amount)
				inventory_changed.emit(items)
				return 0 # Fully added
	
	# If we get here, inventory is full/partial add
	inventory_changed.emit(items)
	return amount # Return remaining amount

func remove_item(index: int, amount: int = 1):
	if index < 0 or index >= max_slots or items[index] == null:
		return
	
	var removed_item = items[index].item
	items[index].amount -= amount
	
	if items[index].amount <= 0:
		items[index] = null
	
	item_removed.emit(removed_item, amount)
	inventory_changed.emit(items)

# --- EQUIPMENT OPS ---
func equip_item(index: int):
	# Move item from inventory to equipment slot
	if index < 0 or index >= max_slots or items[index] == null:
		return
	
	var item = items[index].item
	if not (item is Equipment):
		print("Not equipment!")
		return
	
	var equip = item as Equipment
	var slot = equip.slot
	
	# Remove from inventory (assumes unstackable for equipment usually)
	remove_item(index, 1)
	
	# Unequip existing if any
	if equipment[slot] != null:
		add_item(equipment[slot]) # Return old item to inventory
	
	# Equip new
	equipment[slot] = equip
	equipment_changed.emit(equipment)
	print("Equipped: ", equip.name)

func unequip_item(slot: Equipment.Slot):
	if equipment[slot] == null:
		return
	
	var item = equipment[slot]
	var leftover = add_item(item)
	
	if leftover == 0:
		equipment[slot] = null
		equipment_changed.emit(equipment)
		print("Unequipped: ", item.name)
	else:
		print("Inventory full! Cannot unequip.")
