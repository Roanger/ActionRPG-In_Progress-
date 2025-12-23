class_name Equipment
extends Item

enum Slot {
	CRANIAL, # Helm
	CHASSIS, # Chest
	ACTUATORS, # Bracers
	STRUTS, # Pants
	THRUSTERS, # Boots
	LOGIC_1, # Ring 1
	LOGIC_2, # Ring 2
	REGULATOR, # Belt
	UPLINK, # Necklace
	ARMAMENT # Weapon
}

@export var slot: Slot = Slot.CHASSIS

# Stats Dictionary: key=StatName (String), value=Amount (int/float)
# Example: {"structure": 10, "output": 5}
@export var stats: Dictionary = {
	"structure": 0, # Max HP
	"output": 0, # Base Dmg
	"torque": 0, # Phys Bonus
	"voltage": 0, # Energy Bonus
	"cycle_rate": 0.0, # Atk Speed
	"cooling": 0.0, # Cooldown Reduction
	"velocity": 0.0 # Move Speed
}

@export var level_requirement: int = 1
@export var class_requirement: RobotClass # Optional specific class requirement

func get_stat(stat_name: String) -> float:
	if stats.has(stat_name):
		return stats[stat_name]
	return 0.0
