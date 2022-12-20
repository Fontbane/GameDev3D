#ifndef __GAMELOCAL_H__
#define __GAMELOCAL_H__

#include "gfc_input.h"
#include "gfc_primitives.h"
#include <simple_json.h>
#include <simple_logger.h>

typedef enum GameState {
	Day,
	Night,
	Place,
	Selecting,
	Selected,
	Casting,
	Over
}GameState;

typedef struct GameLocal {
	GameState state;
	Box selection;
	Uint16 name;
	void* selected;
	Vector3D location_chosen;
	Uint8 labBroken : 1;
	Uint8 hutBroken : 1;
	Uint8 padding : 6;
	Uint32 time;
	Uint16 weight;
}GameLocal;

GameLocal game;

#endif // !__GAMELOCAL_H__
