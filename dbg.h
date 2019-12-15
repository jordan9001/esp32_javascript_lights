#ifndef DBG_H
#define DBG_H

#define DEBUG 1
#define dbgf(fmt, ...) \
        do { if (DEBUG) { Serial.printf(fmt, __VA_ARGS__);}} while(0)

#define dbg(str) \
        do { if (DEBUG) Serial.println(str);} while(0)

#define dbg_init() \
        do { if (DEBUG) Serial.begin(9600);} while(0)

#define dbg_flush() \
        do { if (DEBUG) Serial.flush();} while(0)

#endif
