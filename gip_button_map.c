// gip_button_map.c - see gip_button_map.h
//
// The default table below is a transcription of the `if (wButtons & (1 << n))`
// chain that used to live in xinput_host.c, so switching over to the table must
// not change a single button. tests/gip_native checks that by replaying the
// button words the real pad sent.

#include "gip_button_map.h"

#include <string.h>

// Index = bit in the pad's raw button word.
// Value = bit in the XInput wButtons mask that the button ends up as.
//
// XInput bits: 0=DPadUp 1=DPadDown 2=DPadLeft 3=DPadRight 4=Start 5=Back
//              6=LThumb 7=RThumb 8=LB 9=RB 10=Guide 11=Share
//              12=A 13=B 14=X 15=Y
static const uint8_t default_map[GIP_BUTTON_MAP_BITS] = {
    GIP_BUTTON_MAP_UNUSED,  // 0  - unused
    GIP_BUTTON_MAP_UNUSED,  // 1  - unused
    4,                      // 2  - Menu/Start
    5,                      // 3  - View/Back
    12,                     // 4  - A
    13,                     // 5  - B
    14,                     // 6  - X
    15,                     // 7  - Y
    0,                      // 8  - D-pad up
    1,                      // 9  - D-pad down
    2,                      // 10 - D-pad left
    3,                      // 11 - D-pad right
    8,                      // 12 - LB
    9,                      // 13 - RB
    6,                      // 14 - LS (left stick click)
    7,                      // 15 - RS (right stick click)
};

static uint8_t current_map[GIP_BUTTON_MAP_BITS] = {
    GIP_BUTTON_MAP_UNUSED, GIP_BUTTON_MAP_UNUSED,
    4, 5, 12, 13, 14, 15, 0, 1, 2, 3, 8, 9, 6, 7,
};

void gip_button_map_defaults(uint8_t map[GIP_BUTTON_MAP_BITS])
{
    memcpy(map, default_map, GIP_BUTTON_MAP_BITS);
}

bool gip_button_map_validate(const uint8_t map[GIP_BUTTON_MAP_BITS])
{
    uint16_t claimed = 0;
    for (int i = 0; i < GIP_BUTTON_MAP_BITS; i++) {
        if (map[i] == GIP_BUTTON_MAP_UNUSED) {
            continue;
        }
        if (map[i] >= GIP_BUTTON_MAP_BITS) {
            return false;
        }
        uint16_t bit = (uint16_t)(1u << map[i]);
        if (claimed & bit) {
            return false;  // two pad bits cannot both become the same button
        }
        claimed |= bit;
    }
    return true;
}

uint16_t gip_button_map_apply(const uint8_t map[GIP_BUTTON_MAP_BITS],
                              uint16_t raw)
{
    uint16_t out = 0;
    for (int i = 0; i < GIP_BUTTON_MAP_BITS; i++) {
        if (!(raw & (uint16_t)(1u << i))) {
            continue;
        }
        if (map[i] == GIP_BUTTON_MAP_UNUSED || map[i] >= GIP_BUTTON_MAP_BITS) {
            continue;
        }
        out |= (uint16_t)(1u << map[i]);
    }
    return out;
}

const uint8_t* gip_button_map_current(void)
{
    return current_map;
}

bool gip_button_map_set(const uint8_t map[GIP_BUTTON_MAP_BITS])
{
    if (!gip_button_map_validate(map)) {
        return false;
    }
    memcpy(current_map, map, GIP_BUTTON_MAP_BITS);
    return true;
}
