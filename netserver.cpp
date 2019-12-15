#include "networkinfo.h"
#include "netserver.h"
#include "site.h"

#include <pthread.h>
#include <WebServer.h>
#include <WiFi.h>

// Globals
WebServer server(NET_PORT);
char* js1 = NULL;
size_t js1_len = 0;
char* js2 = NULL;
size_t js2_len = 0;
char* currentjs = "";

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
  
  Serial.println("Server starting up");
  // Start Wifi
  
  WiFi.config(ip, gateway, subnet, dns1, dns2);
  
  while (WiFi.begin(NET_SSID, NET_PASS) != WL_CONNECTED) {
    Serial.printf("Connecting to Wifi %s %d...\n", NET_SSID, i);
    Serial.flush();
    delay(1000);
    i++;
  }

  Serial.println(xPortGetCoreID());

  Serial.printf("Connected to Wifi %s\n", NET_SSID);
  Serial.print(WiFi.localIP());
  Serial.flush();

  // allocate the first js
  
  // setup server
  server.on("/", handleIndex);
  server.onNotFound(handleIndex);
  server.on("/update", handleUpdate);
  
  // start server loop thread
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  Serial.println("pthread create started");
  pthread_create(&server_thread, &attr, server_loop, NULL);
  Serial.println("pthread create finished");
}

static void* server_loop(void* arg) {
  server.begin();
  Serial.println("HTTP server up");
  Serial.println(xPortGetCoreID());


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

  Serial.println("Handled Request");
}

static void handleUpdate(void) {
  int i = 0;
  int count = server.args();
  char** jsarea = NULL;
  size_t* jssz = NULL;
  const char* newjs = NULL;
  size_t newsz = 0;

  Serial.println("Getting update");
  if (count != 1) {
    Serial.printf("Got strange count of %d\n", count);
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
    
    if (newsz > *jssz) {
      // allocate area
      *jssz = newsz;
      free(*jsarea);
      *jsarea = (char*)malloc(newsz);
    }

    memcpy(*jsarea, newjs, newsz);
    currentjs = *jsarea;
    Serial.printf("New js %s\n", currentjs);
  }

  server.send(200);
}
