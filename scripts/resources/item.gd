class_name Item
extends Resource

enum Rarity {
	SCRAP, # White
	REFURBISHED, # Green
	TEMPERED, # Blue
	OVERCLOCKED, # Purple
	SENTIENT # Orange
}

@export var id: String = ""
@export var name: String = "Unknown Part"
@export_multiline var description: String = ""
@export var icon: Texture2D
@export var rarity: Rarity = Rarity.SCRAP
@export var max_stack: int = 1
@export var sell_value: int = 10
