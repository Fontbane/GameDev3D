#ifndef __GAMELOCAL_H__
#define __GAMELOCAL_H__

#include "gfc_input.h";
#include <simple_json.h>
#include <simple_logger.h>

typedef enum GameState {
	Day,
	Night,
	Place,
	Selecting,
	Selected
};

#endif // !__GAMELOCAL_H__
