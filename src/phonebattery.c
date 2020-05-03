#include <pebble.h>
#include "configs.h"
#include "phonebattery.h"
#include "keys.h"
#include "text.h"

static bool phonebattery_enabled;
static int last_update = 0;
static int phonebattery_interval = 5;
static int phonebattery_expiration = 30;
static AppTimer *retry_timer;

static void retry_handler(void *context) {
    update_phonebattery(true);
}

void update_phonebattery(bool force) {
  int current_time = (int)time(NULL);
  if (force || last_update == 0 || (current_time - last_update) >= phonebattery_interval * 60) {
    if ((current_time - last_update) >= phonebattery_expiration * 60) {
      set_phonebattery_layer_text("");
    }
    DictionaryIterator *iter;
    AppMessageResult result = app_message_outbox_begin(&iter);
    if (result == APP_MSG_OK) {
      dict_write_uint8(iter, KEY_REQUESTPHONEBATTERY, 1);
      result = app_message_outbox_send();
      if (result == APP_MSG_OK) {
	if (force) {
	  set_phonebattery_layer_text("");
	}
	last_update = current_time;
      }
    } else if (force) {
          retry_timer = app_timer_register(1000, retry_handler, NULL);
        }
  }
}

static bool get_phonebattery_enabled() {
  return is_module_enabled(MODULE_PHONEBATTERY);
}

void update_phonebattery_value(int lvl_val, int chg_val) {
  if (is_module_enabled(MODULE_PHONEBATTERY)) {
    char s_phonebattery_buffer[8];

    if (chg_val) {
      snprintf(s_phonebattery_buffer, sizeof(s_phonebattery_buffer), (lvl_val < 20 ? "!+%d%%" :"+%d%%"), lvl_val);
    } else {
      snprintf(s_phonebattery_buffer, sizeof(s_phonebattery_buffer), (lvl_val < 20 ? "! %d%%" : "%d%%"), lvl_val);
    }
    set_phonebattery_color(lvl_val);
    set_phonebattery_layer_text(s_phonebattery_buffer);
  } else {
    set_phonebattery_layer_text("");
  }
}

void toggle_phonebattery(uint8_t reload_origin) {
    phonebattery_enabled = get_phonebattery_enabled();
    if (reload_origin == RELOAD_CONFIGS || reload_origin == RELOAD_DEFAULT) {
      phonebattery_interval = persist_exists(KEY_PHONEBATTERYTIME) ? persist_read_int(KEY_PHONEBATTERYTIME) : 5;
    }
    if (phonebattery_enabled) {
      if (reload_origin == RELOAD_MODULE || reload_origin == RELOAD_CONFIGS) {
	update_phonebattery(true);
      } else {
	update_phonebattery(false);
      }
    } else {
      set_phonebattery_layer_text("");
    }
}

bool is_phonebattery_enabled() {
  return phonebattery_enabled;
}
