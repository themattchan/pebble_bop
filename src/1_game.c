//#include <pebble.h>
#include "1_game.h"

static Window *window;
static TextLayer *text_layer;
static AppTimer *timer;
static void timer_callback(void *data);

int count = 0;
double time_interval = 5000;

STATE mState = start;

ACTION mAction = none;			/* Action we want */
ACTION mGesture = none;			/* Action that user inputs */

/* Need implementations for unused handlers */
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {}
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {}

/* START WITH MIDDLE BUTTON PUSH */
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (mState == start) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "select: start > pick_action");
		mState = pick_action;
		state();
	} else if (mState == end) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "select: end > start");
		APP_LOG(APP_LOG_LEVEL_DEBUG, "select: action > none");
		APP_LOG(APP_LOG_LEVEL_DEBUG, "select: gesture > none");
		mState = start;
		mAction = none;
		mGesture = none;

		count = 0;
		time_interval = 5000;

		text_layer_set_text(text_layer, "START");
	} else {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "select: not in valid state");
	}
}

static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

/* Setup UI -- Window handlers for load and unload */
static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	text_layer = text_layer_create((GRect) {
			.origin = { 0, 60 },
			.size = { bounds.size.w, 40 }
		});
	// set text attributes
	text_layer_set_text(text_layer, "START");
	text_layer_set_text_color(text_layer, GColorBlack);
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	// set background color
	text_layer_set_background_color(text_layer, GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(text_layer));
	// turn backlight on
	light_enable(true);
}

static void window_unload(Window *window) {
	text_layer_destroy(text_layer);
}

/* Our logic for setup and teardown -- call in the main driver */
void bop_init(void) {
	window = window_create();
	window_set_click_config_provider(window, click_config_provider);
	window_set_window_handlers(window, (WindowHandlers) {
			.load = window_load,
			.unload = window_unload
	});
	const bool animated = true;
	window_stack_push(window, animated);

	accel_tap_service_subscribe(accel_tap_handler);
}

void bop_deinit(void) {
	accel_tap_service_unsubscribe();
	window_destroy(window);
}

static void timer_callback(void *data) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "timer: state > check");
	mState = check;
	state();
}

/* GAME LOGIC */
/* State machine control flow */
void state(void) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "in state machine");
	switch (mState) {
	case pick_action:
		APP_LOG(APP_LOG_LEVEL_DEBUG, "machine: pick action");
		handle_pick_action(); break;
	case check:
		APP_LOG(APP_LOG_LEVEL_DEBUG, "machine: check");
		handle_check(); break;
	case end:
		APP_LOG(APP_LOG_LEVEL_DEBUG, "machine: end");
		handle_end(); break;
	default:
		text_layer_set_text(text_layer, "ERROR"); break;
	}
}

void accel_tap_handler(AccelAxisType axis, int32_t direction) {
	if (mState == pick_action) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "tap event registered");
		switch (axis) {
		case ACCEL_AXIS_X:
			mGesture = pull;
			text_layer_set_text(text_layer, "pulled");
			APP_LOG(APP_LOG_LEVEL_DEBUG, "tap event = pulled");
			break;
		case ACCEL_AXIS_Y:
			mGesture = twist;
			text_layer_set_text(text_layer, "twisted");
			APP_LOG(APP_LOG_LEVEL_DEBUG, "tap event = twisted");
			break;
		case ACCEL_AXIS_Z:
			mGesture = bop;
			text_layer_set_text(text_layer, "bopped");
			APP_LOG(APP_LOG_LEVEL_DEBUG, "tap event = bopped");
			break;
		default:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "tap event error");
			break;
		}
	}
}

void handle_pick_action(void) {
	static int randInt = -1;
	if (randInt == -1)
		srand(time(NULL));
	randInt = rand() % 3;
	switch (randInt) {
	case 0:
		mAction = bop;
		text_layer_set_text(text_layer, "BOP");
		APP_LOG(APP_LOG_LEVEL_DEBUG, "rand_action: bop");
		break;
	case 1:
		mAction = pull;
		text_layer_set_text(text_layer, "PULL");
		APP_LOG(APP_LOG_LEVEL_DEBUG, "rand_action: pull");
		break;
	case 2:
		mAction = twist;
		text_layer_set_text(text_layer, "TWIST");
		APP_LOG(APP_LOG_LEVEL_DEBUG, "rand_action: twist");
		break;
	default:
		APP_LOG(APP_LOG_LEVEL_DEBUG, "rand_action: error");
		return;
	}
	// haptic feedback for new action
	vibes_short_pulse();
	// (re)start Timer
	APP_LOG(APP_LOG_LEVEL_DEBUG, "pick_action: timer started");
	timer = app_timer_register(time_interval, timer_callback, NULL);
}

void handle_check(void) {
	if(mAction == mGesture){ //success
		APP_LOG(APP_LOG_LEVEL_DEBUG, "check: success, state > pick_action");
		count++;
		time_interval-=500;
		mState = pick_action;
	} else { //fail
		APP_LOG(APP_LOG_LEVEL_DEBUG, "check: fail, state > end");
		mState = end;
	}
	state();
}


void handle_end(void) {
	char scoreTmp[sizeof(int)];
	snprintf(scoreTmp, sizeof(int), "%d", count);
	const char score[sizeof(int)];
	APP_LOG(APP_LOG_LEVEL_DEBUG, "made it to end. Score:");
	APP_LOG(APP_LOG_LEVEL_DEBUG, score);

	text_layer_set_text(text_layer, score);

	vibes_double_pulse();
}
