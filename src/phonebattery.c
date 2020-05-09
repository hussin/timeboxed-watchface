#include <pebble.h>
#include "configs.h"
#include "phonebattery.h"
#include "keys.h"
#include "text.h"

#if !defined PBL_PLATFORM_APLITE

static bool phonebattery_enabled;
static bool force_update = false;
static int last_update = 0;
static int phonebattery_interval = 5;
/* exp time == 90 + 10 extra (because in timeboxed.c time when pebble 
health available delay may be up to 90 minutes) */
static int phonebattery_expiration = 100;
static int phonebattery_level;
static int phonebattery_charging;

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

void update_phonebattery_from_storage(){
  if (persist_exists(KEY_PHONEBATTERY_LEVEL) && persist_exists(KEY_PHONEBATTERY_CHARGING)){
    phonebattery_charging = persist_read_int(KEY_PHONEBATTERY_CHARGING);
    phonebattery_level = persist_read_int(KEY_PHONEBATTERY_LEVEL);
    update_phonebattery_value(phonebattery_level, phonebattery_charging);
  }
}

void store_phonebattery_vals(int lvl_val, int chg_val){
  persist_write_int(KEY_PHONEBATTERY_CHARGING, chg_val);
  persist_write_int(KEY_PHONEBATTERY_LEVEL, lvl_val);
}

void toggle_phonebattery(uint8_t reload_origin) {
    phonebattery_enabled = is_module_enabled(MODULE_PHONEBATTERY);
    if (reload_origin == RELOAD_CONFIGS || reload_origin == RELOAD_DEFAULT) {
      phonebattery_interval = persist_exists(KEY_PHONEBATTERYTIME) ? persist_read_int(KEY_PHONEBATTERYTIME) : 5;
    }
    if (phonebattery_enabled) {
      update_phonebattery_from_storage();
      if (reload_origin == RELOAD_MODULE || reload_origin == RELOAD_CONFIGS) {
	force_update = true;
      }
    } else {
      set_phonebattery_layer_text("");
    }
}

bool is_phonebattery_need_update() {
    int current_time = (int)time(NULL);
    bool fup = force_update;
    force_update = false;
    if ((current_time - last_update) >= phonebattery_expiration * 60) {
      set_phonebattery_layer_text("");
    }
    return (phonebattery_enabled && (fup || last_update == 0 || (current_time - last_update) >= phonebattery_interval * 60));
}

void phonebattery_set_updatetime(int updtime) {
  last_update = updtime;
}

#endif
