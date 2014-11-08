#include <pebble.h>
#include "1_game.h"

static void init(void) {
	bop_init();
}

static void deinit(void) {
	bop_deinit();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
