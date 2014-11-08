#include <pebble.h>
#include "1_game.h"

static Window *window;
static TextLayer *text_layer;
static AppTimer *timer;

static int randInt = -1;

int count = 0;
double time_interval = 5000;

STATE mState = start;

ACTION mAction = none;
ACTION mGesture = none;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (mState == start) {
		mState = pick_action;
		state();
	} else if (mState == end) {
		mState = start;
		count = 0;
		time_interval = 5000;
		text_layer_set_text(text_layer, "START");
	}
}

/* Need implementations for unused handlers */
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {}
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {}

static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	text_layer = text_layer_create((GRect) {
			.origin = { 0, 60 },
			.size = { bounds.size.w, 40 }
		});
	//set text attributes
	text_layer_set_text(text_layer, "START");
	text_layer_set_text_color(text_layer, GColorBlack);
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	//set background color
	text_layer_set_background_color(text_layer, GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(text_layer));
	//turn backlight on
	light_enable(true);
}

static void window_unload(Window *window) {
	text_layer_destroy(text_layer);
}

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
	window_destroy(window);
	accel_tap_service_unsubscribe();
}

void state(void) {
	switch (mState) {
	case pick_action:
		handle_pick_action(); break;
	case check:
		handle_check(); break;
	case end:
		handle_end(); break;
	default:
		text_layer_set_text(text_layer, "ERROR"); break;
	}
}

void accel_tap_handler(AccelAxisType axis, int32_t direction) {
	if(mState==start)
		return;
	switch (axis) {
		case ACCEL_AXIS_X:
			mGesture = pull;
			text_layer_set_text(text_layer, "pull!");
			break;
		case ACCEL_AXIS_Y:
			mGesture = twist;
			text_layer_set_text(text_layer, "twist!");
			break;
		case ACCEL_AXIS_Z:
			mGesture = bop;
			text_layer_set_text(text_layer, "bop!");
			break;
		default:
			break;
	}
}

void handle_end(void) {
	/*char score[sizeof(int)];
	snprintf(score, sizeof(int), "%d", count);*/
	text_layer_set_text(text_layer, "END");
	
	vibes_double_pulse();
}

void handle_pick_action(void) {
	if (randInt == -1)
		srand(time(NULL));
	randInt = rand()%3;
	switch (randInt) {
	case bop:
		mAction = bop;
		text_layer_set_text(text_layer, "BOP");
		break;
	case pull:
		mAction = pull;
		text_layer_set_text(text_layer, "PULL");
		break;
	case twist:
		mAction = twist;
		text_layer_set_text(text_layer, "TWIST");
		break;
	default:
		return;
	}
	//haptic feedback for new action
	vibes_short_pulse();
	//(re)start Timer
	timer = app_timer_register(time_interval, timer_callback, NULL);
}

void handle_check(void) {
	if(mAction == mGesture){ //success
		count++;
		time_interval-=500;
		mState = pick_action;
	} else { //fail
		mState = end;
	}
	state();
}

static void timer_callback(void *data) {
	mState = check;
	state();
}
