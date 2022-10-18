#ifndef __SAVE_H__
#define __SAVE_H__

#include "gfc_types.h"
#include "gfc_list.h"
#include "building.h"

#define SF_UNLOCKED_DOUBLE_CANNON		0x0001
#define SF_UNLOCKED_GIANT_CANNON		0x0002
#define SF_UNLOCKED_MULTI_ROCKET		0x0004
#define SF_UNLOCKED_STICKY_ROCKET		0x0008
#define SF_UNLOCKED_MULTI_MORTAR		0x0010
#define SF_UNLOCKED_MORTAR_BLOWUP		0x0020
#define SF_UNLOCKED_IVORY_TOWER			0x0040
#define SF_UNLOCKED_OBSIDIAN_TOWER		0x0080
#define SF_UNLOCKED_INFERNO_MELTDOWN	0x0100
#define SF_UNLOCKED_INFERNO_HEATSOAK	0x0200
#define SF_UNLOCKED_ELECTRO_WALL		0x0400
#define	SF_UNLOCKED_SLUDGE_WALL			0x0800

typedef struct PlayerData {
	Uint16 enemiesKilled;
	Uint32 gold;
	Uint32 elixir;
	Uint8 day;
	Uint8 totalBuildings;
	Uint8 totalWalls;
	Uint8 totalCannons;
	Uint8 totalRockets;
	Uint8 totalMortars;
	Uint8 totalWizardTowers;
	Uint8 totalInfernoTowers;
	Uint8 hallLevel;
	Uint16 upgradeflags;
	List* buildings;
} PlayerData;



#endif /*__SAVE_H__*/