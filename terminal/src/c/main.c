#include <pebble.h>
#include "main.h"

static Window *s_main_window;

static GBitmap *s_terminal_bitmap;
static BitmapLayer *s_bitmap_layer;

static TextLayer *s_textdate_layer;

static TextLayer *s_time_layer;
static TextLayer *s_date_layer;

static TextLayer *s_textinfo_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_connection_layer;
static TextLayer *s_step_layer;

static TextLayer *s_textroot_layer;

static char PersistDisconnectVibrate[] = "0";  //<================
static char PersistDate[] = "0";  //<================
static char PersistComputerType[] = "0";  //<================

//settings
// A struct for our specific settings (see main.h)
ClaySettings settings;

// Initialize the default settings
static void prv_default_settings() {
  settings.BackgroundColor = GColorBlack;
  settings.TextColor = GColorWhite;
  strcpy(PersistDisconnectVibrate, "0");
  strcpy(PersistDate, "0");
  strcpy(PersistComputerType,  "0");              //<================
}

// Read settings from persistent storage
static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
  
  if(persist_exists(MESSAGE_KEY_ComputerType)) {                        //<================
     persist_read_string(MESSAGE_KEY_ComputerType, PersistComputerType, sizeof(PersistComputerType));
      APP_LOG(APP_LOG_LEVEL_INFO, "In Load Settings : PersistComputerType = %s", PersistComputerType);
   }
    if(persist_exists(MESSAGE_KEY_DisconnectVibrate)) {                        //<================
     persist_read_string(MESSAGE_KEY_DisconnectVibrate, PersistDisconnectVibrate, sizeof(PersistDisconnectVibrate));
   }
    if(persist_exists(MESSAGE_KEY_Date)) {                        //<================
     persist_read_string(MESSAGE_KEY_Date, PersistDate, sizeof(PersistDate));
   }
}
// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
    APP_LOG(APP_LOG_LEVEL_INFO, "In Save settings: PersistComputerType = %s", PersistComputerType);
  persist_write_string(MESSAGE_KEY_ComputerType,   PersistComputerType);  //<================
  persist_write_string(MESSAGE_KEY_DisconnectVibrate, PersistDisconnectVibrate);
  persist_write_string(MESSAGE_KEY_Date, PersistDate);
  // Update the display based on new settings
  prv_update_display();
}

static void background(){
  gbitmap_destroy(s_terminal_bitmap);
  APP_LOG(APP_LOG_LEVEL_INFO, "In BG: PersistComputerType = %s", PersistComputerType);
  
  if(strcmp(PersistComputerType, "0")==0) {//<================
     s_terminal_bitmap = gbitmap_create_with_resource(RESOURCE_ID_WindowsTerminal_Image);
  } else {  //<================
     s_terminal_bitmap = gbitmap_create_with_resource(RESOURCE_ID_MacTerminal_Image);
  } 
  bitmap_layer_set_bitmap(s_bitmap_layer, s_terminal_bitmap);
}

// Update the display elements
static void prv_update_display() {
  // Background color
  window_set_background_color(s_main_window, settings.BackgroundColor);
  
  background();

  // Text Color
  text_layer_set_text_color(s_textdate_layer, settings.TextColor);
  text_layer_set_text_color(s_textinfo_layer, settings.TextColor);
  text_layer_set_text_color(s_textroot_layer, settings.TextColor);
  text_layer_set_text_color(s_time_layer, settings.TextColor);
  text_layer_set_text_color(s_date_layer, settings.TextColor);
  text_layer_set_text_color(s_battery_layer, settings.TextColor);
  text_layer_set_text_color(s_connection_layer, settings.TextColor);
  text_layer_set_text_color(s_step_layer, settings.TextColor);
  
  //text
  static char textdate_text[] = "root@Pebble:/$ date";
  snprintf(textdate_text, sizeof(textdate_text), "root@Pebble:/$ date");
  text_layer_set_text(s_textdate_layer, textdate_text);
  static char textinfo_text[] = "root@Pebble:/$ info";
  snprintf(textinfo_text, sizeof(textinfo_text), "root@Pebble:/$ info");
  text_layer_set_text(s_textinfo_layer, textinfo_text);
   static char textroot_text[] = "root@Pebble:/$";
  snprintf(textroot_text, sizeof(textroot_text), "root@Pebble:/$");
  text_layer_set_text(s_textroot_layer, textroot_text);
  
    // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[10];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  // Display this time on the TextLayer
  clock_copy_time_string(s_buffer,sizeof(s_buffer));
  text_layer_set_text(s_time_layer, s_buffer);
  
  // Show the date
  if(strcmp(PersistDate, "0")== 0) {
    static char date_buffer[40];
    strftime(date_buffer, sizeof(date_buffer), "%a %b %d", tick_time);
    text_layer_set_text(s_date_layer, date_buffer);
  } else if(strcmp(PersistDate, "1") == 1) {
    static char date_buffer[40];
    strftime(date_buffer, sizeof(date_buffer), "%b %d, %Y", tick_time );
    text_layer_set_text(s_date_layer, date_buffer);    
  } else if(strcmp(PersistDate, "2") == 2) {
    static char date_buffer[40];
    strftime(date_buffer, sizeof(date_buffer), "%D", tick_time );
    text_layer_set_text(s_date_layer, date_buffer);  
  } else if (strcmp(PersistDate, "3") == 3) {
    static char date_buffer[40];
    strftime(date_buffer, sizeof(date_buffer), "%A %D", tick_time );
    text_layer_set_text(s_date_layer, date_buffer);  
  } else if (strcmp(PersistDate, "4")== 4) {
    static char date_buffer[40];
    strftime(date_buffer, sizeof(date_buffer), "%a %D", tick_time );
    text_layer_set_text(s_date_layer, date_buffer);  
  }  
}


// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
    //Computer Image Type
  Tuple *type_select_t = dict_find(iter, MESSAGE_KEY_ComputerType);  
  
	if(type_select_t) { 
     strcpy(PersistComputerType, type_select_t->value->cstring); //<================
  } else {
     strcpy(PersistComputerType, "0"); //<================
  }
    APP_LOG(APP_LOG_LEVEL_INFO, "In InboxReceived: PersistComputerType = %s", PersistComputerType);

  // Background Color
  Tuple *bg_color_t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  if (bg_color_t) {
    settings.BackgroundColor = GColorFromHEX(bg_color_t->value->int32);
  }
  // Text Color
  Tuple *fg_color_t = dict_find(iter, MESSAGE_KEY_TextColor);
  if (fg_color_t) {
    settings.TextColor = GColorFromHEX(fg_color_t->value->int32);
  }
   // Disconnect Vibrations
  Tuple *disconnect_vibratate_t = dict_find(iter, MESSAGE_KEY_DisconnectVibrate);
  if (disconnect_vibratate_t) {
      strcpy(PersistDisconnectVibrate, disconnect_vibratate_t->value->cstring);
  } else {
      strcpy(PersistDisconnectVibrate, "0");
  }
  //Date
  Tuple *bp_select_t = dict_find(iter, MESSAGE_KEY_Date);
	if(bp_select_t) { 
      strcpy(PersistDate, bp_select_t->value->cstring); 
  } else {
      strcpy(PersistDate, "0");
  }
  // Save the new settings to persistent storage
  prv_save_settings();
}

static void get_step_count() {
static char s_current_steps_buffer[20];
static int s_step_count = 0;
s_step_count = (int)health_service_sum_today(HealthMetricStepCount);
int thousands = s_step_count/1000;;
int hundreds = s_step_count-(thousands*1000);
  if(thousands > 0) {
    snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),"Step Count: %d,%d", thousands, hundreds);
  } else {
    snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),"Step Count: %d", s_step_count);
  }
  text_layer_set_text(s_step_layer, s_current_steps_buffer);
}

// pebble health handler
static void health_handler(HealthEventType event, void *context) {
  if(event != HealthEventSleepUpdate) {
    get_step_count();
  }
}


//pebble time handler
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  prv_update_display();
}

//battery 
static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "Battery: Charging";
  if (charge_state.is_charging) {
    snprintf(battery_text, sizeof(battery_text), "Battery: Charging");
  } else {
    snprintf(battery_text, sizeof(battery_text), "Battery: %d%%", charge_state.charge_percent);
  }
  text_layer_set_text(s_battery_layer, battery_text);
}

//bluetooth
static void handle_bluetooth(bool connected) {
  text_layer_set_text(s_connection_layer, connected ? "Bluetooth: Yes" : "Bluetooth: No");
}

static void bluetooth_callback(bool connected) {
	if(!connected) {
			if(strcmp(PersistDisconnectVibrate, "0") == 0) {
      }								// No vibration 
			else if(strcmp(PersistDisconnectVibrate, "1") == 1) {
        vibes_short_pulse(); 
      }		// Short vibration
			else if(strcmp(PersistDisconnectVibrate, "2") == 2) {
        vibes_long_pulse(); 
      }		// Long vibration
			else if(strcmp(PersistDisconnectVibrate, "3") == 3) { 
        vibes_double_pulse(); 
      }	// Double vibration					
	} 
}


static void prv_main_window_load(Window *window) {
  int screen;
  int screenInc;

    screen=20;
    screenInc=15;

  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
 
  //Image
  s_terminal_bitmap = gbitmap_create_with_resource(RESOURCE_ID_WindowsTerminal_Image);
  s_bitmap_layer = bitmap_layer_create(GRect(0, 0, 144, 23));
  bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_bitmap_layer, s_terminal_bitmap);
  
  //bluetooth vibration
  bluetooth_callback(connection_service_peek_pebble_app_connection()); 
  
  s_textdate_layer = text_layer_create(GRect(0, screen, bounds.size.w, 35));
  text_layer_set_background_color(s_textdate_layer, GColorClear);
  text_layer_set_text_alignment(s_textdate_layer, GTextAlignmentLeft);
  text_layer_set_text(s_textdate_layer, "root@Pebble:/$ date"); 
  screen= screen + screenInc;
  

  //time
  s_time_layer = text_layer_create( GRect(0, PBL_IF_ROUND_ELSE(screen, screen), bounds.size.w, 35));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
  screen= screen + screenInc;
  
  //date
  s_date_layer = text_layer_create(GRect(0, screen, bounds.size.w, 35));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
  text_layer_set_text(s_date_layer, "September 00, 0000\nWednesday");

  
  
  screen= screen + 25;
   s_textinfo_layer = text_layer_create(GRect(0, screen, bounds.size.w, 35));
  text_layer_set_background_color(s_textinfo_layer, GColorClear);
  text_layer_set_text_alignment(s_textinfo_layer, GTextAlignmentLeft);
  text_layer_set_text(s_textinfo_layer, "root@Pebble:/$ info"); 
  screen= screen + screenInc;

  //battery
  s_battery_layer = text_layer_create(GRect(0, screen, bounds.size.w, 35));
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentLeft);
  text_layer_set_text(s_battery_layer, "Battery: 100%"); 
  battery_state_service_subscribe(handle_battery);
  handle_battery(battery_state_service_peek());
  screen= screen + screenInc;
  
 
  //bluetooth
  s_connection_layer = text_layer_create(GRect(0, screen, bounds.size.w, 35));
  text_layer_set_background_color(s_connection_layer, GColorClear);
  text_layer_set_text_alignment(s_connection_layer, GTextAlignmentLeft);
  handle_bluetooth(connection_service_peek_pebble_app_connection());
  bluetooth_callback(connection_service_peek_pebble_app_connection());
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = handle_bluetooth
  });
  screen= screen + screenInc;
 
  // Subscribe to health events if we can
    health_service_events_subscribe(health_handler, NULL);
  //steps
  s_step_layer = text_layer_create(GRect(0, screen, bounds.size.w, 35));
  text_layer_set_background_color(s_step_layer, GColorClear);
  text_layer_set_text_alignment(s_step_layer, GTextAlignmentLeft);
  text_layer_set_text(s_step_layer, "Steps: 0"); 
  screen= screen + screenInc;
  
  
  screen= screen + 10;
  s_textroot_layer = text_layer_create(GRect(0, screen, bounds.size.w, 35));
  text_layer_set_background_color(s_textroot_layer, GColorClear);
  text_layer_set_text_alignment(s_textroot_layer, GTextAlignmentLeft);
  text_layer_set_text(s_textroot_layer, "root@Pebble:/$"); 
  
  

    text_layer_set_font(s_textdate_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
    text_layer_set_font(s_textinfo_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    text_layer_set_font(s_connection_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    text_layer_set_font(s_step_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
    text_layer_set_font(s_textroot_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  
  //child layers
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_textdate_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_textinfo_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_textroot_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_connection_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_step_layer));
  
  prv_update_display();
}

static void prv_main_window_unload(Window *window) {
  
  //Destroy Images
  gbitmap_destroy(s_terminal_bitmap);
  bitmap_layer_destroy(s_bitmap_layer);
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_connection_layer);
  text_layer_destroy(s_textdate_layer);
  text_layer_destroy(s_textinfo_layer);
  text_layer_destroy(s_textroot_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_step_layer);
  
  //unsubscribe
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
}

static void prv_init() {
  prv_load_settings();

  // Listen for AppMessages
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(256, 256);
  
   // Create main Window element and assign to pointer
  s_main_window = window_create();
 // window_set_background_color(window, GColorBlack);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = prv_main_window_load,
    .unload = prv_main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
 
  // Register with TickTimerService
tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  //subscribe to heart rate
 // health_service_events_subscribe(prv_on_health_data, NULL);
  
  // Make sure the time/text is displayed from the start
  prv_update_display();

}

static void prv_deinit() {
    // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}