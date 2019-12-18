#include "networkinfo.h"
#include "netserver.h"
#include "site.h"
#include "dbg.h"

#include <pthread.h>
#include <WebServer.h>
#include <WiFi.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

// Globals
WebServer server(NET_PORT);
char* js1 = NULL;
size_t js1_len = 0;
char* js2 = NULL;
size_t js2_len = 0;
char* currentjs = "delay(1500);";
char ledstate = 1;

// static helpers
static void* server_loop(void* arg);
static void handleIndex(void);
static void handleUpdate(void);

char* get_js() {
  //TODO should probably use a MUX here
  return currentjs;
}

int netserver_setup() {
  int i = 0;
  pthread_attr_t attr;
  pthread_t server_thread;

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  ledstate = 1;
  
  dbg(_T, "Server starting up");
  // Start Wifi
  
  WiFi.config(ip, gateway, subnet, dns1, dns2);
  
  while (WiFi.begin(NET_SSID, NET_PASS) != WL_CONNECTED) {
    dbgf(_T, "Connecting to Wifi %s %d...\n", NET_SSID, i);
    Serial.flush();
    delay(1000);
    i++;
  }

  dbgf(_T, "Connected to Wifi %s\n", NET_SSID);
  dbg(_T, WiFi.localIP());

  // allocate the first js
  
  // setup server
  server.on("/", handleIndex);
  server.onNotFound(handleIndex);
  server.on("/update", handleUpdate);
  
  // start server loop thread
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&server_thread, &attr, server_loop, NULL);
}

static void* server_loop(void* arg) {
  server.begin();
  dbg(_T, "HTTP server up");


  while (true) {
    // need to give up cpu time to other loop
    server.handleClient();
    
    delay(100);
  }
  
  return NULL;
}

static void handleIndex(void) {
  String response = SITESTART;
  response += get_js();
  response += SITEEND;

  server.send(200, "text/html", response);

  dbg(_D, "Handled Request");

  ledstate = !ledstate;
  digitalWrite(LED_BUILTIN, (ledstate) ? HIGH : LOW);
}

static void handleUpdate(void) {
  int i = 0;
  int count = server.args();
  char** jsarea = NULL;
  size_t* jssz = NULL;
  const char* newjs = NULL;
  size_t newsz = 0;

  dbg(_D, "Getting update");
  if (count != 1) {
    dbgf(_W, "Got strange count of %d\n", count);
  } else {
    // swap back and forth, working on one while the other is used
    //TODO really should have some mux though
    if (currentjs == js1) {
      jsarea = &js2;
      jssz = &js2_len;
    } else {
      jsarea = &js1;
      jssz = &js1_len;
    }
  
    newjs = server.arg(i).c_str();
    newsz = strlen(newjs)+1;
    dbgf(_W, "newsz %x\n", newsz);
    dbg_flush();
    
    if (newsz > *jssz) {
      // allocate area
      *jssz = newsz;
      if (*jsarea != 0) {
        free(*jsarea);
      }
      *jsarea = (char*)malloc(newsz);
    }

    memcpy(*jsarea, newjs, newsz);
    currentjs = *jsarea;
    dbgf(_D, "New js %s\n", currentjs);
  }

  server.send(200);
  ledstate = !ledstate;
  digitalWrite(LED_BUILTIN, (ledstate) ? HIGH : LOW);
}
