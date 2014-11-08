#include <pebble.h>
#include "bop.h"

static Window *window;
static TextLayer *text_layer;
static AppTimer *timer;

int time_interval = 4000;
int count = 0;

STATE mState = start;
ACTION mAction = none;

//GAME INIT
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	//text_layer_set_text(text_layer, "Select");
	if (mState == start) {
		mState = pick_action;
		state();
	}
}

/* Need implementations for unused handlers */
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	/* text_layer_set_text(text_layer, "Up"); */
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	/* text_layer_set_text(text_layer, "Down"); */
}

static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	text_layer = text_layer_create((GRect) {
			.origin = { 0, 72 },
				.size = { bounds.size.w, 20 } });
	//set text attributes
	text_layer_set_text(text_layer, "START");
	text_layer_set_text_color(text_layer, GColorBlack);
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	//set background color
	text_layer_set_background_color(text_layer, GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
	text_layer_destroy(text_layer);
}

static void init(void) {
	window = window_create();
	window_set_click_config_provider(window, click_config_provider);
	window_set_window_handlers(window, (WindowHandlers)
							   {load = window_load, .unload = window_unload});
	const bool animated = true;
	window_stack_push(window, animated);

	//Start Accel Data Service
	handle_init();
}

static void deinit(void) {
	handle_deinit();
	window_destroy(window);
}

void handle_init(void) {
	// Passing 0 to subscribe sets up the accelerometer for peeking
	accel_data_service_subscribe(0, NULL);
}

void handle_deinit(void) {
	accel_data_service_unsubscribe();
}

int main(void) {
	init();

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

	app_event_loop();
	deinit();
}

//typedef enum {start, pick_action, check, update} STATE;

void state(void) {
	switch (mState) {
	case pick_action:
		handle_pick_action(); break;
	case check:
		handle_check(); break;
	case update:
		handle_update(); break;
	case end:
		handle_end(); break;
	default:
		break;
	}
}

void handle_end(void) {
	char *score = atoa(count);
	text_layer_set_text(text_layer, score);
}

void handle_update(void) {
	//update game variables
	count++;
	time_interval/=10;
	
	mState = pick_action;
	state();
}

void handle_pick_action(void) {
	srand(time(NULL));
	mAction = rand()%3;
	switch (mAction) {
	case bop:
		text_layer_set_text(text_layer, "BOP");
		break;
	case pull:
		text_layer_set_text(text_layer, "PULL");
		break;
	case twist:
		text_layer_set_text(text_layer, "TWIST");
		break;
	default:
		return;
	}
	//turn on backlight for action instr
	light_enable_interaction();
	//haptic feedback for new action
	vibes_short_pulse();
	//(re)start Timer
	timer = app_timer_register(time_interval, timer_callback, NULL);
}

void handle_check(void) {
	/* AccelData data; */
	/* accel_service_peek(&data); */
	if (true){
		mState = update;
		state();
	} else {
		mState = end;
		state();
	}
	/* //Resets Timer */
	/* timer = app_timer_register(time_interval, timer_callback, NULL); */
}

static void timer_callback(void *data) {
	if(mState == pick_action){
		mState = check;
		state();
	}
}
