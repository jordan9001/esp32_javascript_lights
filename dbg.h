#ifndef DBG_H
#define DBG_H

#define _D          4
#define _T          3
#define _W          2
#define _E          1
#define _SILENT     0

#define DEBUG_LVL   0


#define dbgf(lvl, fmt, ...) \
        do { if (lvl <= DEBUG_LVL) { Serial.printf(fmt, __VA_ARGS__);}} while(0)

#define dbg(lvl, str) \
        do { if (lvl <= DEBUG_LVL) Serial.println(str);} while(0)

#define dbg_init() \
        do { if (DEBUG_LVL > _SILENT) Serial.begin(9600);} while(0)

#define dbg_flush() \
        do { if (DEBUG_LVL > _SILENT) Serial.flush();} while(0)

#endif
