#include <pebble.h>
#include "configs.h"
#include "crypto.h"
#include "keys.h"
#include "text.h"

#if !defined PBL_PLATFORM_APLITE

static bool crypto_enabled;
static bool force_update = false;
static int last_update = 0;
static int crypto_interval = 15;
static char price[8];
static char price_b[8];
static char price_c[8];
static char price_d[8];

static bool get_crypto_enabled() {
    return is_module_enabled(MODULE_CRYPTO) ||
        is_module_enabled(MODULE_CRYPTOB) ||
        is_module_enabled(MODULE_CRYPTOC) ||
        is_module_enabled(MODULE_CRYPTOD);
}

void update_crypto_price(char* price) {
    if (is_module_enabled(MODULE_CRYPTO)) {
        set_crypto_layer_text(price);
    } else {
        set_crypto_layer_text("");
    }
}

void update_crypto_price_b(char* price) {
    if (is_module_enabled(MODULE_CRYPTOB)) {
        set_crypto_b_layer_text(price);
    } else {
        set_crypto_b_layer_text("");
    }
}

void update_crypto_price_c(char* price) {
    if (is_module_enabled(MODULE_CRYPTOC)) {
        set_crypto_c_layer_text(price);
    } else {
        set_crypto_c_layer_text("");
    }
}

void update_crypto_price_d(char* price) {
    if (is_module_enabled(MODULE_CRYPTOD)) {
        set_crypto_d_layer_text(price);
    } else {
        set_crypto_d_layer_text("");
    }
}

static void update_crypto_from_storage() {
    if (persist_exists(KEY_CRYPTOPRICE)) {
        persist_read_string(KEY_CRYPTOPRICE, price, sizeof(price));
        update_crypto_price(price);
    }
    if (persist_exists(KEY_CRYPTOPRICEB)) {
        persist_read_string(KEY_CRYPTOPRICEB, price_b, sizeof(price_b));
        update_crypto_price_b(price_b);
    }
    if (persist_exists(KEY_CRYPTOPRICEC)) {
        persist_read_string(KEY_CRYPTOPRICEC, price_c, sizeof(price_c));
        update_crypto_price_c(price_c);
    }
    if (persist_exists(KEY_CRYPTOPRICED)) {
        persist_read_string(KEY_CRYPTOPRICED, price_d, sizeof(price_d));
        update_crypto_price_d(price_d);
    }
}

void store_crypto_price(char* price) {
    persist_write_string(KEY_CRYPTOPRICE, price);
}

void store_crypto_price_b(char* price) {
    persist_write_string(KEY_CRYPTOPRICEB, price);
}

void store_crypto_price_c(char* price) {
    persist_write_string(KEY_CRYPTOPRICEC, price);
}

void store_crypto_price_d(char* price) {
    persist_write_string(KEY_CRYPTOPRICED, price);
}

void toggle_crypto(uint8_t reload_origin) {
    crypto_enabled = get_crypto_enabled();
    if (reload_origin == RELOAD_CONFIGS || reload_origin == RELOAD_DEFAULT) {
        crypto_interval = persist_exists(KEY_CRYPTOTIME) ? persist_read_int(KEY_CRYPTOTIME) : 15;
    }
    if (crypto_enabled) {
        update_crypto_from_storage();
        if (reload_origin == RELOAD_MODULE || reload_origin == RELOAD_CONFIGS) {
	  force_update = true;
        }
    } else {
        set_crypto_layer_text("");
        set_crypto_b_layer_text("");
        set_crypto_c_layer_text("");
        set_crypto_d_layer_text("");
    }
}

bool is_crypto_need_update() {
  int current_time = (int)time(NULL);
  bool fup = force_update;
  force_update = false;
  crypto_enabled = get_crypto_enabled();
  return  (crypto_enabled && (fup || last_update == 0 || (current_time - last_update) >= crypto_interval * 60));
}

void crypto_set_updatetime(int updtime) {
  last_update = updtime;
}
#endif
