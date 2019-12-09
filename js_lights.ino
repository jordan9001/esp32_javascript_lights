#include <Arduino.h>
#include <pthread.h>
#include <esp_pthread.h>
#include "duktape.h"

#define PTHREAD_CREATE_DETACHED 0

duk_context *dctx;


void setup() {
  pthread_attr_t attr;
  pthread_t server_thread;

  // start serial communication
  Serial.begin(9600);
  
  // start server thread
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&server_thread, &attr, server, NULL);

  // setup duktape
  dctx = duk_create_heap_default();
  if (!dctx) {
    Serial.println("Unable to start up duk!");
  }

  duk_push_c_function(dctx, native_debug, DUK_VARARGS);
  duk_put_global_string(dctx, "print");

  Serial.println("All Setup");
}

static void* server(void* arg) {
  Serial.println("Server starting up");
  return NULL;
}

static duk_ret_t native_debug(duk_context* ctx) {
  Serial.print("Duk Debug : ");
  Serial.println(duk_safe_to_string(ctx, 0));
  return 0;
}

void loop() {
  
  // check for update javascript
  //TODO
  
  // do loop for lights with mjs
  //TODO

  Serial.println("---");
  duk_eval_string(dctx, "print('HELLO WORLD ' + ((42 * 31) + 35));");
  delay(1000);
  
}
