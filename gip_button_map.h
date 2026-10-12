// gip_button_map.h - Which XInput button each raw GIP button bit stands for.
//
// The Xbox One pad sends a 16-bit button word in every input report. Which
// physical button a bit stands for is a property of the pad, not of the
// protocol: the PDP wired pad this was read off puts the D-pad on bits 8-11 and
// the shoulders/stick clicks on bits 12-15, which is not what the usual Xbox
// layout would suggest.
//
// That word used to be decoded by a chain of hardcoded `if (wButtons & (1<<n))`
// tests, so correcting it for a different pad meant a reflash. This module
// turns it into a table that can be rewritten at runtime and kept in flash.
//
// The table is deliberately a pure function of its arguments - no flash, no
// globals - so tests/gip_native can compile it on the host and check every
// entry against what the real pad reported.

#ifndef GIP_BUTTON_MAP_H
#define GIP_BUTTON_MAP_H

#include <stdint.h>
#include <stdbool.h>

// One entry per bit of the pad's raw button word.
#define GIP_BUTTON_MAP_BITS 16

// An entry that stands for no button at all. Bits 0 and 1 of the word are
// unused on every pad seen so far.
#define GIP_BUTTON_MAP_UNUSED 0xFF

// Fill *map with the layout the firmware has always used (the PDP pad the
// hardcoded chain was written against).
void gip_button_map_defaults(uint8_t map[GIP_BUTTON_MAP_BITS]);

// True if *map can be used: every entry is either UNUSED or a bit index, and no
// two live entries claim the same output bit.
bool gip_button_map_validate(const uint8_t map[GIP_BUTTON_MAP_BITS]);

// Translate a raw button word into an XInput button mask using *map.
uint16_t gip_button_map_apply(const uint8_t map[GIP_BUTTON_MAP_BITS],
                              uint16_t raw);

// The map currently in force elsewhere in the firmware.
const uint8_t* gip_button_map_current(void);

// Replace the map in force. Rejected if it does not validate, in which case the
// previous map stays in force.
bool gip_button_map_set(const uint8_t map[GIP_BUTTON_MAP_BITS]);

#endif // GIP_BUTTON_MAP_H
