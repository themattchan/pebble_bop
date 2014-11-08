private typedef enum {start, pick_action, check, update, end} STATE;
private typedef enum {none, bop, pull, twist} ACTION;

private void state(void);
private void handle_pick_action(void);
private void handle_check(void);
private void handle_update(void);
private void handle_end(void);
private static void timer_callback(void *data);

private void accel_tap_handler(AccelAxisType axis, int32_t direction);

public bop_init();
public bop_deinit();

