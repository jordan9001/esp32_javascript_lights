#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "WS2812_ESP_RMT.h"
#include "netserver.h"

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

  // start serial communication
  Serial.begin(9600);

  // start the server
  netserver_setup();

  Serial.println("Net server setup");
  Serial.flush();

  // setup duktape
  dctx = duk_create_heap_default();
  if (!dctx) {
    Serial.println("Unable to start up duk!");
  }

  duk_push_c_function(dctx, native_debug, DUK_VARARGS);
  duk_put_global_string(dctx, "print");

  duk_push_c_function(dctx, native_delay, DUK_VARARGS);
  duk_put_global_string(dctx, "delay");

  Serial.println("All Setup");
  Serial.flush();
}

static duk_ret_t native_debug(duk_context* ctx) {
  Serial.print("Duk Debug : ");
  Serial.println(duk_safe_to_string(ctx, 0));
  return 0;
}

static duk_ret_t native_delay(duk_context* ctx) {
  uint16_t amt = duk_to_uint16(ctx, 0);
  Serial.printf("native delay %d\n", amt);
  delay(amt);
  return 0;
}

void loop() {
  char* js = NULL;
  
  // check for updated javascript
  js = get_js();
  

  Serial.printf("Evaluating %s\n", js);
  duk_eval_string(dctx, js);
  
}
