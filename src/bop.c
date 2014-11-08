#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static AppTimer *timer;

int time_interval = 10000;

typedef enum {start, pick_action, check, update} STATE;
STATE mState = start;

typedef enum {bop, pull, twist} ACTION;
ACTION mAction = NULL;


static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(text_layer, "Select");
  if (mState == start) {  
	//Start Accel Data Service
	handle_init();
	
	mState = pick_action;
	pick_action();
	
	//Start Timer
	timer = app_timer_register(time_interval, timer_callback, NULL);
  }
}

/*static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
}*/

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
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
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  handle_deinit();
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}

void update_ui_from_accel(void) {
  AccelData data;
  accel_service_peek(&data);
  // Insert UI code here
}
void handle_init(void) {
  // Passing 0 to subscribe sets up the accelerometer for peeking
  accel_data_service_subscribe(0, NULL);
}
void handle_deinit(void) {
  accel_data_service_unsubscribe();
}

static void timer_callback(void *data) {
  //Insert Check State Here
  
  //Resets Timer
  timer = app_timer_register(time_interval, timer_callback, NULL);
}

void pick_action(void) {
	srand(time(NULL));
	int actionNum = rand()%3;
	mAction = actionNum
	switch (actionNum) {
		case 0:
			//bop display to screen
			mAction = bop;
			break;
		case 1:
			//pull;
			mAction = pull;
			break;
		case 2:
			//twist
			mAction = twist;
			break;
		default:
			break;
	}
	
}



