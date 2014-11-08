#include <pebble.h>
#include "bop.h"

static void init(void) {
	bop_init();
}

static viod deinit(void) {
	bob_deinit();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}