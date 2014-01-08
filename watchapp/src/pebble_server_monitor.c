/* Pebble Server Monitor
    Written by edenist
    20140107

    robotprofessor.net
*/

#include <pebble.h>

#define DEFAULT_HOST "DEFAULT_HOST"

#define DEFAULT_BAR_VAL 40

typedef Layer ProgressBarLayer;

static Window *window;
static TextLayer *hostname_text;
static TextLayer *cpu_usage_text;
static TextLayer *mem_usage_text;
static TextLayer *ip_text;
static TextLayer *debug_text;
static ProgressBarLayer *progress_bar_cpu;
static ProgressBarLayer *progress_bar_mem;
static char host[16];
static char cpu[10];
static char mem[10];
static char ip[16];

enum {
  SERVER_KEY_FETCH = 0x0,
  SERVER_KEY_IP = 0x1,
  SERVER_KEY_CPU = 0x2,
  SERVER_KEY_MEM = 0x3,
  SERVER_KEY_HOST = 0x4,
};

typedef struct {
  unsigned int progress; // how full the bar is
} ProgressData;

static void set_ip_msg(char *ip) {
  Tuplet ip_tuple = TupletCString(SERVER_KEY_IP, ip);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &ip_tuple);
  dict_write_end(iter);
  
  app_message_outbox_send();
}

static void fetch_msg(void) {
  Tuplet fetch_tuple = TupletInteger(SERVER_KEY_FETCH, 1);
  Tuplet cpu_tuple = TupletInteger(SERVER_KEY_CPU, 1);
  Tuplet mem_tuple = TupletInteger(SERVER_KEY_MEM, 1);
  Tuplet host_tuple = TupletInteger(SERVER_KEY_HOST, 1);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &fetch_tuple);
  dict_write_tuplet(iter, &cpu_tuple);
  dict_write_tuplet(iter, &mem_tuple);
  dict_write_tuplet(iter, &host_tuple);
  dict_write_end(iter);

  app_message_outbox_send();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //refresh usage
  text_layer_set_text(debug_text, "Refreshing...");
  fetch_msg();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(debug_text, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(debug_text, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void updateBarData(ProgressBarLayer *bar, int newVal) {
  ProgressData *data = layer_get_data(bar);
  data->progress = newVal;
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *ip_tuple = dict_find(iter, SERVER_KEY_IP);
  Tuple *cpu_tuple = dict_find(iter, SERVER_KEY_CPU);
  Tuple *mem_tuple = dict_find(iter, SERVER_KEY_MEM);
  Tuple *host_tuple = dict_find(iter, SERVER_KEY_HOST);

  if (ip_tuple) {
    strncpy(ip, ip_tuple->value->cstring, 16);
    text_layer_set_text(ip_text, ip);
  }
  if (cpu_tuple) {
    strncpy(cpu, cpu_tuple->value->cstring, 10);
    text_layer_set_text(cpu_usage_text, cpu);
    updateBarData(progress_bar_cpu, atoi(cpu));
    layer_mark_dirty(progress_bar_cpu);
  }
  if (mem_tuple) {
    strncpy(mem, mem_tuple->value->cstring, 10);
    text_layer_set_text(mem_usage_text, mem);
    updateBarData(progress_bar_mem, atoi(mem));
    layer_mark_dirty(progress_bar_mem);
  }
  if (host_tuple) {
    strncpy(host, host_tuple->value->cstring, 16);
    text_layer_set_text(hostname_text, host);
  }
}

static void in_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Dropped!");
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Failed to Send!");
}

static void app_message_init(void) {
  // Register message handlers
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_failed(out_failed_handler);
  // Init buffers
  app_message_open(64, 64);
  fetch_msg();
}

static void progress_bar_layer_update(ProgressBarLayer *bar, GContext *ctx) {
  ProgressData *data = (ProgressData *)layer_get_data(bar);
  
  // Outline the progress bar
  graphics_context_set_stroke_color(ctx, GColorBlack);
  GRect bounds = layer_get_bounds(bar);
  graphics_draw_round_rect(ctx, GRect(0, 0, bounds.size.w, bounds.size.h), 4);
  
  // Fill the progress bar
  graphics_context_set_fill_color(ctx, GColorBlack);
  //graphics_fill_rect(ctx, GRect(0, 0, data->progress, bounds.size.h), 4, GCornersAll);
  graphics_fill_rect(ctx, GRect(0, 0, data->progress, bounds.size.h), 4, GCornersAll);
}

static ProgressBarLayer * progress_bar_layer_create(int y_pos) {
  ProgressBarLayer *progress_bar_layer = layer_create_with_data(GRect(6, y_pos, 130, 18), sizeof(ProgressData));
  layer_set_update_proc(progress_bar_layer, progress_bar_layer_update);
  layer_mark_dirty(progress_bar_layer);

  return progress_bar_layer;
}

static void progress_bar_destroy(ProgressBarLayer *progress_bar_layer) {
  layer_destroy(progress_bar_layer);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  
  //hostname text layer
  hostname_text = text_layer_create(GRect(0, 0, 144, 20));
  layer_add_child(window_layer, text_layer_get_layer(hostname_text));
  text_layer_set_text(hostname_text, DEFAULT_HOST);

  //cpu layers
  cpu_usage_text = text_layer_create(GRect(0, 20, 144, 20));
  layer_add_child(window_layer, text_layer_get_layer(cpu_usage_text));
  text_layer_set_text(cpu_usage_text, "CPU Usage");
  progress_bar_cpu = progress_bar_layer_create(40);     //function takes y-pos as argument
  layer_add_child(window_layer, progress_bar_cpu);

  //mem layers
  mem_usage_text = text_layer_create(GRect(0, 60, 144, 20));
  layer_add_child(window_layer, text_layer_get_layer(mem_usage_text));
  text_layer_set_text(mem_usage_text, "Mem Usage");
  progress_bar_mem = progress_bar_layer_create(80);
  layer_add_child(window_layer, progress_bar_mem);

  //ip text layer
  ip_text = text_layer_create(GRect(0, 100, 144, 20));
  layer_add_child(window_layer, text_layer_get_layer(ip_text));
  text_layer_set_text(ip_text, "IP Address");

  //debug text layer 
  debug_text = text_layer_create(GRect(50, 120, 94, 20));
  layer_add_child(window_layer, text_layer_get_layer(debug_text));
  text_layer_set_text(debug_text, "DEBUG");

  fetch_msg();
}

static void window_unload(Window *window) {
  text_layer_destroy(hostname_text);
  text_layer_destroy(cpu_usage_text);
  text_layer_destroy(mem_usage_text);
  text_layer_destroy(ip_text);
  text_layer_destroy(debug_text);
  progress_bar_destroy(progress_bar_cpu);
  progress_bar_destroy(progress_bar_mem);
}

static void init(void) {
  window = window_create();
  app_message_init();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
  
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
