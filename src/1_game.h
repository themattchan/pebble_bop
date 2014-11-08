#include <pebble.h>

typedef enum {start, pick_action, check, end} STATE;
typedef enum {none, bop, pull, twist} ACTION;

void state(void);
void handle_pick_action(void);
void handle_check(void);
void handle_end(void);
static void timer_callback(void *data);

void accel_tap_handler(AccelAxisType axis, int32_t direction);

void bop_init(void);
void bop_deinit(void);
