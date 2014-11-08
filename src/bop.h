typedef enum {start, pick_action, check, update, end} STATE;
typedef enum {none, bop, pull, twist} ACTION;

void state(void);
void handle_pick_action(void);
void handle_check(void);
void handle_update(void);
void handle_end(void);
void handle_init(void);
void handle_deinit(void);
static void timer_callback(void *data);

void accel_tap_handler(AccelAxisType axis, int32_t direction);
