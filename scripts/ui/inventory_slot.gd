extends Button

signal clicked(index)

var slot_index: int = -1
var item_data = null

@onready var icon_rect = $TextureRect
@onready var amount_label = $Label

func init(data, index):
	slot_index = index
	item_data = data
	
	if data:
		var item = data.item
		# Set Icon (needs item.icon to be valid Texture2D)
		if item.icon:
			icon_rect.texture = item.icon
		
		# Set Amount
		if data.amount > 1:
			amount_label.text = str(data.amount)
		else:
			amount_label.text = ""
			
		# Optional: Tooltip
		tooltip_text = item.name + "\n" + item.description
	else:
		icon_rect.texture = null
		amount_label.text = ""
		tooltip_text = "Empty"

func _pressed():
	clicked.emit(slot_index)
