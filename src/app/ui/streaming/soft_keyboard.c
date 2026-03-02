/*
 * Soft keyboard overlay (Xbox-style) - sends keys directly to streaming host.
 * Toggle modifiers (Shift, Ctrl, Alt, Win) for combos like Alt+F4, Win+G.
 */

#include "soft_keyboard.h"
#include "app.h"
#include "config.h"
#include "stream/input/session_input.h"
#include "stream/input/vk.h"
#include "stream/session.h"

#include <Limelight.h>
#include <string.h>

#define MOD_SHIFT 0x01
#define MOD_CTRL  0x02
#define MOD_ALT   0x04
#define MOD_META  0x08

/* Modifier VKs that toggle; use left-side variants for simplicity */
#define VK_SHIFT      VK_LSHIFT
#define VK_CTRL       VK_LCONTROL
#define VK_ALT        VK_LMENU
#define VK_CLOSE_BTN  0  /* Sentinel for close button in btnmatrix */

typedef struct {
    lv_obj_t *container;
    lv_group_t *group;
    stream_input_t *input;
    bool toggle_shift;
    bool toggle_ctrl;
    bool toggle_alt;
    bool toggle_win;
    void (*on_close)(void *);
    void *on_close_userdata;
} soft_kbd_t;

static char mods_to_flags(soft_kbd_t *kbd) {
    char m = 0;
    if (kbd->toggle_shift) m |= MODIFIER_SHIFT;
    if (kbd->toggle_ctrl)  m |= MODIFIER_CTRL;
    if (kbd->toggle_alt)   m |= MODIFIER_ALT;
    if (kbd->toggle_win && app_configuration->syskey_capture) m |= MODIFIER_META;
    return m;
}

static void send_key(soft_kbd_t *kbd, short vk, bool down) {
    stream_input_send_key_event(kbd->input, vk, down, mods_to_flags(kbd));
}

static void release_toggles(soft_kbd_t *kbd) {
    if (kbd->toggle_shift) { kbd->toggle_shift = false; send_key(kbd, VK_LSHIFT, false); }
    if (kbd->toggle_ctrl)  { kbd->toggle_ctrl = false;  send_key(kbd, VK_LCONTROL, false); }
    if (kbd->toggle_alt)   { kbd->toggle_alt = false;   send_key(kbd, VK_LMENU, false); }
    if (kbd->toggle_win)   { kbd->toggle_win = false;   send_key(kbd, VK_LWIN, false); }
}

typedef struct {
    soft_kbd_t *kbd;
    short *vks;
    int vk_count;
} kbd_data_t;

static void on_keyboard_click(lv_event_t *e) {
    kbd_data_t *kd = lv_event_get_user_data(e);
    uint32_t *btn_id_ptr = lv_event_get_param(e);
    uint16_t btn_id = (btn_id_ptr != NULL) ? (uint16_t) *btn_id_ptr : lv_btnmatrix_get_selected_btn(lv_event_get_target(e));
    if (btn_id == LV_BTNMATRIX_BTN_NONE) return;
    /* Row 4+ (indices 48+) report btn_id + 1 due to column misalignment between rows */
    uint16_t vk_idx = (btn_id >= 48) ? btn_id - 1 : btn_id;
    if (vk_idx >= (uint16_t) kd->vk_count) return;
    short vk = kd->vks[vk_idx];
    if (vk == 0 || vk == VK_CLOSE_BTN) {
        if (vk == VK_CLOSE_BTN) {
            soft_kbd_t *kbd = kd->kbd;
            release_toggles(kbd);
            if (kbd->on_close) kbd->on_close(kbd->on_close_userdata);
        }
        return;
    }

    soft_kbd_t *kbd = kd->kbd;
    if (vk == VK_SHIFT || vk == VK_RSHIFT) {
        kbd->toggle_shift = !kbd->toggle_shift;
        send_key(kbd, vk, kbd->toggle_shift);
        return;
    }
    if (vk == VK_CTRL || vk == VK_RCONTROL) {
        kbd->toggle_ctrl = !kbd->toggle_ctrl;
        send_key(kbd, vk, kbd->toggle_ctrl);
        return;
    }
    if (vk == VK_ALT || vk == VK_RMENU) {
        kbd->toggle_alt = !kbd->toggle_alt;
        send_key(kbd, vk, kbd->toggle_alt);
        return;
    }
    if ((vk == VK_LWIN || vk == VK_RWIN) && app_configuration->syskey_capture) {
        kbd->toggle_win = !kbd->toggle_win;
        send_key(kbd, vk, kbd->toggle_win);
        return;
    }
    send_key(kbd, vk, true);
    send_key(kbd, vk, false);
    release_toggles(kbd);
}

static void kbd_data_delete_cb(lv_event_t *e) {
    kbd_data_t *kd = lv_event_get_user_data(e);
    if (kd && kd->vks) free(kd->vks);
    if (kd) free(kd);
}

static void container_delete_cb(lv_event_t *e) {
    soft_kbd_t *kbd = lv_event_get_user_data(e);
    if (kbd) {
        if (kbd->group) {
            lv_group_del(kbd->group);
        }
        free(kbd);
    }
}

lv_group_t *soft_keyboard_get_group(lv_obj_t *keyboard_container) {
    soft_kbd_t *kbd = lv_obj_get_user_data(keyboard_container);
    return kbd ? kbd->group : NULL;
}

/* Single btnmatrix: close + keys, unified D-pad navigation (no Select needed) */
static const char *kbd_map[] = {
    "X","ESC","F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12",
    "\n",
    "`","1","2","3","4","5","6","7","8","9","0","-","=", LV_SYMBOL_BACKSPACE, "Ins","Home","PgUp",
    "\n",
    "Tab","Q","W","E","R","T","Y","U","I","O","P","[","]","\\","Del","End","PgDn",
    "\n",
    "Caps","A","S","D","F","G","H","J","K","L",";","'", LV_SYMBOL_NEW_LINE,
    "\n",
    "Shift","Z","X","C","V","B","N","M",",",".","/","Shift", "\xE2\x86\x91", /* ↑ */
    "\n",
    "Ctrl","Win","Alt","Space","AltGr","Win","Menu","Ctrl",
    "\n",
    LV_SYMBOL_LEFT, LV_SYMBOL_DOWN, LV_SYMBOL_RIGHT,
    ""
};

static const short kbd_vks[] = {
    VK_CLOSE_BTN,VK_ESCAPE,VK_F1,VK_F2,VK_F3,VK_F4,VK_F5,VK_F6,VK_F7,VK_F8,VK_F9,VK_F10,VK_F11,VK_F12,
    VK_OEM_3,VK_1,VK_2,VK_3,VK_4,VK_5,VK_6,VK_7,VK_8,VK_9,VK_0,VK_OEM_MINUS,VK_OEM_PLUS,VK_BACK,VK_INSERT,VK_HOME,VK_PRIOR,
    VK_TAB,VK_Q,VK_W,VK_E,VK_R,VK_T,VK_U,VK_I,VK_O,VK_P,VK_OEM_4,VK_OEM_6,VK_OEM_5,VK_DELETE,VK_END,VK_NEXT,
    VK_CAPITAL,VK_A,VK_S,VK_D,VK_F,VK_G,VK_H,VK_J,VK_K,VK_L,VK_OEM_1,VK_OEM_7,VK_RETURN,
    VK_SHIFT,VK_Z,VK_X,VK_C,VK_V,VK_B,VK_N,VK_M,VK_OEM_COMMA,VK_OEM_PERIOD,VK_OEM_2,VK_SHIFT,VK_UP,
    VK_LCONTROL,VK_LWIN,VK_ALT,VK_SPACE,VK_RMENU,VK_RWIN,VK_APPS,VK_RCONTROL,
    VK_LEFT,VK_DOWN,VK_RIGHT
};

#define KBD_VK_COUNT ((int)(sizeof(kbd_vks)/sizeof(kbd_vks[0])))

lv_obj_t *soft_keyboard_create(lv_obj_t *parent, session_t *session,
                               void (*on_close_cb)(void *userdata), void *userdata) {
    stream_input_t *input = session_get_input(session);

    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_70, 0);

    soft_kbd_t *kbd = malloc(sizeof(soft_kbd_t));
    if (!kbd) return cont;
    memset(kbd, 0, sizeof(*kbd));
    kbd->container = cont;
    kbd->input = input;
    kbd->on_close = on_close_cb;
    kbd->on_close_userdata = userdata;
    kbd->group = lv_group_create();
    lv_obj_set_user_data(cont, kbd);

    /* Keyboard content container: fixed % height for 4K, centered, all 7 rows visible */
    lv_obj_t *kbd_content = lv_obj_create(cont);
    lv_obj_remove_style_all(kbd_content);
    lv_obj_set_width(kbd_content, LV_PCT(100));
    lv_obj_set_height(kbd_content, lv_pct(35));  /* 35% of screen - scales with 4K */
    lv_obj_set_style_pad_all(kbd_content, LV_DPX(10), 0);
    lv_obj_set_style_pad_gap(kbd_content, LV_DPX(4), 0);
    lv_obj_set_flex_flow(kbd_content, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(kbd_content, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(kbd_content, LV_ALIGN_BOTTOM_MID, 0, -LV_DPX(24));  /* Near bottom of screen */

    /* Single btnmatrix: close (X) + keys - D-pad navigates all, no Select needed */
    kbd_data_t *kd = malloc(sizeof(kbd_data_t));
    if (!kd) { lv_obj_del(cont); return cont; }
    kd->kbd = kbd;
    kd->vk_count = KBD_VK_COUNT;
    kd->vks = malloc((size_t)KBD_VK_COUNT * sizeof(short));
    if (!kd->vks) { free(kd); lv_obj_del(cont); return cont; }
    memcpy(kd->vks, kbd_vks, (size_t)KBD_VK_COUNT * sizeof(short));

    lv_obj_t *btnm = lv_btnmatrix_create(kbd_content);
    lv_group_add_obj(kbd->group, btnm);
    lv_btnmatrix_set_map(btnm, kbd_map);
    lv_obj_set_width(btnm, LV_PCT(100));
    lv_obj_set_flex_grow(btnm, 1);  /* Fill remaining space in kbd_content - all 7 rows visible */
    lv_obj_set_style_pad_all(btnm, LV_DPX(2), 0);
    lv_obj_set_style_pad_gap(btnm, LV_DPX(2), 0);
    lv_obj_set_style_bg_opa(btnm, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(btnm, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(btnm, LV_OPA_30, LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(btnm, lv_palette_main(LV_PALETTE_BLUE), LV_STATE_PRESSED);
    lv_obj_set_style_outline_width(btnm, LV_DPX(2), LV_STATE_FOCUSED);
    lv_obj_set_style_outline_color(btnm, lv_palette_main(LV_PALETTE_BLUE), LV_STATE_FOCUSED);
    lv_obj_add_event_cb(btnm, on_keyboard_click, LV_EVENT_VALUE_CHANGED, kd);
    lv_obj_add_event_cb(btnm, kbd_data_delete_cb, LV_EVENT_DELETE, kd);
    lv_btnmatrix_set_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_CLICK_TRIG | LV_BTNMATRIX_CTRL_NO_REPEAT);

    lv_group_focus_obj(btnm);
    lv_obj_add_event_cb(cont, container_delete_cb, LV_EVENT_DELETE, kbd);

    return cont;
}
