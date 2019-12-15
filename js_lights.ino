#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "WS2812_ESP_RMT.h"
#include "netserver.h"
#include "dbg.h"

// other definitions
#define PTHREAD_CREATE_DETACHED 0

/* NOTE, these duk files are not included in the repo, see https://github.com/svaarala/duktape */
#include "duktape.h"

// layout config
#define PIN                     22  // data pin for leds
#define NUMPIXELS               150

// Globals
duk_context *dctx; // global javascript context
ESPRMTLED pixels = ESPRMTLED(NUMPIXELS, PIN, RMT_CHANNEL_0);

void setup() {

  pixels.clear();

  dbg_init();

  // start the server
  netserver_setup();

  dbg("Net server setup");

  // setup duktape
  dctx = duk_create_heap_default();
  if (!dctx) {
    dbg("Unable to start up duk!");
  }

  // setup duktape environment
  duk_push_c_function(dctx, native_debug, DUK_VARARGS);
  duk_put_global_string(dctx, "print");

  duk_push_c_function(dctx, native_delay, 1);
  duk_put_global_string(dctx, "delay");

  duk_push_c_function(dctx, native_led_set, 2);
  duk_put_global_string(dctx, "setled");
  
  duk_push_c_function(dctx, native_led_clear, 0);
  duk_put_global_string(dctx, "clearled");

  duk_push_c_function(dctx, native_led_show, 0);
  duk_put_global_string(dctx, "showled");

  dbg("All Setup");
}

static duk_ret_t native_led_set(duk_context* ctx) {
  uint16_t index = 0;
  uint32_t color = 0;

  index = (uint16_t)duk_get_uint(ctx, -2);
  color = (uint32_t)duk_get_uint(ctx, -1);

  dbgf("Setting %d to 0x%08x\n", index, color);

  pixels.setPixelColor(index, color);

  return 0;
}

static duk_ret_t native_led_clear(duk_context* ctx) {
  pixels.clear();
  return 0;
}

static duk_ret_t native_led_show(duk_context* ctx) {
  pixels.show();
  return 0;
}

static duk_ret_t native_debug(duk_context* ctx) {
  dbg("Duk Debug : ");
  dbg(duk_safe_to_string(ctx, 0));
  return 0;
}

static duk_ret_t native_delay(duk_context* ctx) {
  uint16_t amt = duk_to_uint16(ctx, 0);
  dbgf("native delay %d\n", amt);
  delay(amt);
  return 0;
}

void loop() {
  char* js = NULL;
  
  // check for updated javascript
  js = get_js();

  duk_eval_string(dctx, js);
}
