#ifndef _WLC_H_
#define _WLC_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#if __GNUC__
#  define WLC_LOG_ATTR(x, y) __attribute__((format(printf, x, y)))
#else
#  define WLC_LOG_ATTR(x, y)
#endif

struct wlc_compositor;
struct wlc_view;
struct wlc_output;
struct wlc_space;
struct wl_list;

/** wlc_log, wlc_vlog() */
enum wlc_log_type {
   WLC_LOG_INFO,
   WLC_LOG_WARN,
   WLC_LOG_ERROR,
};

/** wlc_view_get_state(); */
enum wlc_view_bit {
   WLC_BIT_MAXIMIZED = 1<<0,
   WLC_BIT_FULLSCREEN = 1<<1,
   WLC_BIT_RESIZING = 1<<2,
   WLC_BIT_ACTIVATED = 1<<3,
};

/** mods in interface.keyboard.key function */
enum wlc_modifier_bit {
   WLC_BIT_MOD_SHIFT = 1<<0,
   WLC_BIT_MOD_CAPS = 1<<1,
   WLC_BIT_MOD_CTRL = 1<<2,
   WLC_BIT_MOD_ALT = 1<<3,
   WLC_BIT_MOD_MOD2 = 1<<4,
   WLC_BIT_MOD_MOD3 = 1<<5,
   WLC_BIT_MOD_LOGO = 1<<6,
   WLC_BIT_MOD_MOD5 = 1<<7,
};

/** leds in interface.keyboard.key function */
enum wlc_led_bit {
   WLC_BIT_LED_NUM = 1<<0,
   WLC_BIT_LED_CAPS = 1<<1,
   WLC_BIT_LED_SCROLL = 1<<2,
};

/** state in interface.keyboard.key function */
enum wlc_key_state {
   WLC_KEY_STATE_RELEASED = 0,
   WLC_KEY_STATE_PRESSED = 1
};

/** state in interface.pointer.button function */
enum wlc_button_state {
   WLC_BUTTON_STATE_RELEASED = 0,
   WLC_BUTTON_STATE_PRESSED = 1
};

struct wlc_interface {
   struct {
      bool (*created)(struct wlc_compositor*, struct wlc_view*, struct wlc_space*);
      void (*destroyed)(struct wlc_compositor*, struct wlc_view*);
      void (*will_move_to_space)(struct wlc_compositor*, struct wlc_view*, struct wlc_space*);

      struct {
         void (*geometry)(struct wlc_compositor*, struct wlc_view*, int32_t x, int32_t y, uint32_t width, uint32_t height);
         void (*state)(struct wlc_compositor*, struct wlc_view*, enum wlc_view_bit state, bool toggle);
      } request;
   } view;

   struct {
      void (*init)(struct wlc_compositor*, struct wlc_view*);
      bool (*key)(struct wlc_compositor*, struct wlc_view*, uint32_t leds, uint32_t mods, uint32_t key, enum wlc_key_state state);
   } keyboard;

   struct {
      bool (*button)(struct wlc_compositor*, struct wlc_view*, uint32_t button, enum wlc_button_state state);
      bool (*motion)(struct wlc_compositor*, struct wlc_view*, int32_t x, int32_t y);
   } pointer;

   struct {
      bool (*created)(struct wlc_compositor*, struct wlc_output*);
      void (*destroyed)(struct wlc_compositor*, struct wlc_output*);
      void (*activated)(struct wlc_compositor*, struct wlc_output*);
      void (*resolution)(struct wlc_compositor*, struct wlc_output*, uint32_t width, uint32_t height);
   } output;

   struct {
      bool (*created)(struct wlc_compositor*, struct wlc_space*);
      void (*destroyed)(struct wlc_compositor*, struct wlc_space*);
      void (*activated)(struct wlc_compositor*, struct wlc_space*);
   } space;
};

bool wlc_init(const int argc, char *argv[]);
void wlc_set_log_file(FILE *out);
WLC_LOG_ATTR(2, 3) void wlc_log(const enum wlc_log_type type, const char *fmt, ...);
void wlc_vlog(const enum wlc_log_type type, const char *fmt, va_list ap);

void wlc_output_set_resolution(struct wlc_output *output, uint32_t width, uint32_t height);
void wlc_output_get_resolution(struct wlc_output *output, uint32_t *out_width, uint32_t *out_height);
struct wlc_space* wlc_output_get_active_space(struct wlc_output *output);
struct wl_list* wlc_output_get_spaces(struct wlc_output *output);
struct wl_list* wlc_output_get_link(struct wlc_output *output);
struct wlc_output* wlc_output_from_link(struct wl_list *output_link);
void wlc_output_set_userdata(struct wlc_output *output, void *userdata);
void* wlc_output_get_userdata(struct wlc_output *output);
void wlc_output_focus_space(struct wlc_output *output, struct wlc_space *space);

struct wlc_output* wlc_space_get_output(struct wlc_space *space);
struct wl_list* wlc_space_get_views(struct wlc_space *space);
struct wl_list* wlc_space_get_link(struct wlc_space *space);
struct wlc_space* wlc_space_from_link(struct wl_list *space_link);
void wlc_space_set_userdata(struct wlc_space *space, void *userdata);
void* wlc_space_get_userdata(struct wlc_space *space);
void wlc_space_remove(struct wlc_space *space);
struct wlc_space* wlc_space_add(struct wlc_output *output);

void wlc_view_set_title(struct wlc_view *view, const char *title);
void wlc_view_set_class(struct wlc_view *view, const char *_class);
void wlc_view_set_space(struct wlc_view *view, struct wlc_space *space);
struct wlc_space* wlc_view_get_space(struct wlc_view *view);
uint32_t wlc_view_get_state(struct wlc_view *view);
void wlc_view_set_state(struct wlc_view *view, enum wlc_view_bit state, bool toggle);
void wlc_view_resize(struct wlc_view *view, uint32_t width, uint32_t height);
void wlc_view_position(struct wlc_view *view, int32_t x, int32_t y);
void wlc_view_close(struct wlc_view *view);
void wlc_view_send_below(struct wlc_view *view, struct wlc_view *below);
void wlc_view_send_to_back(struct wlc_view *view);
void wlc_view_bring_above(struct wlc_view *view, struct wlc_view *above);
void wlc_view_bring_to_front(struct wlc_view *view);
struct wl_list* wlc_view_get_link(struct wlc_view *view);
struct wlc_view* wlc_view_from_link(struct wl_list *view_link);
struct wl_list* wlc_view_get_user_link(struct wlc_view *view);
struct wlc_view* wlc_view_from_user_link(struct wl_list *view_link);
void wlc_view_set_userdata(struct wlc_view *view, void *userdata);
void* wlc_view_get_userdata(struct wlc_view *view);

struct wl_list* wlc_compositor_get_outputs(struct wlc_compositor *compositor);
struct wlc_output* wlc_compositor_get_focused_output(struct wlc_compositor *compositor);
struct wlc_space* wlc_compositor_get_focused_space(struct wlc_compositor *compositor);

void wlc_compositor_focus_view(struct wlc_compositor *compositor, struct wlc_view *view);
void wlc_compositor_focus_output(struct wlc_compositor *compositor, struct wlc_output *output);

void wlc_compositor_run(struct wlc_compositor *compositor);
void wlc_compositor_free(struct wlc_compositor *compositor);
struct wlc_compositor* wlc_compositor_new(const struct wlc_interface *interface);

/**
 * Macros for wl_list containers containing wlc structs.
 *
 * _user versions of each macro cycles the *_get_user_link() version of each struct.
 * User versions of the links are not touched by the wlc at all.
 *
 * You should only use the non user version of the links for enumerating wl_lists returned by some wlc functions.
 * Never insert/remove the non user links.
 */

#define wlc_space_for_each(item, list)                             \
        for (item = wlc_space_from_link((list)->next);             \
             wlc_space_get_link(item) != (list);                   \
             item = wlc_space_from_link(wlc_space_get_link(item)->next))

#define wlc_space_for_each_reverse(item, list)                     \
        for (item = wlc_space_from_link((list)->prev);             \
             wlc_space_get_link(item) != (list);                   \
             item = wlc_space_from_link(wlc_space_get_link(item)->prev))

#define wl_space_for_each_safe(item, tmp, list)                    \
        for (item = wlc_space_from_link((list)->next),             \
             tmp = wlc_space_from_link((list)->next->next);        \
             wlc_space_get_link(item) != (list);                   \
             item = tmp,                                           \
             tmp = wlc_space_from_link(wlc_space_get_link(item)->next))

#define wl_space_for_each_safe_reverse(item, tmp, list)            \
        for (item = wlc_space_from_link((list)->prev),             \
             tmp = wlc_space_from_link((list)->prev->prev);        \
             wlc_space_get_link(item) != (list);                   \
             item = tmp,                                           \
             tmp = wlc_space_from_link(wlc_space_get_link(item)->prev))

#define wlc_output_for_each(item, list)                             \
        for (item = wlc_output_from_link((list)->next);             \
             wlc_output_get_link(item) != (list);                   \
             item = wlc_output_from_link(wlc_output_get_link(item)->next))

#define wlc_output_for_each_reverse(item, list)                     \
        for (item = wlc_output_from_link((list)->prev);             \
             wlc_output_get_link(item) != (list);                   \
             item = wlc_output_from_link(wlc_output_get_link(item)->prev))

#define wl_output_for_each_safe(item, tmp, list)                    \
        for (item = wlc_output_from_link((list)->next),             \
             tmp = wlc_output_from_link((list)->next->next);        \
             wlc_output_get_link(item) != (list);                   \
             item = tmp,                                            \
             tmp = wlc_output_from_link(wlc_output_get_link(item)->next))

#define wl_output_for_each_safe_reverse(item, tmp, list)            \
        for (item = wlc_output_from_link((list)->prev),             \
             tmp = wlc_output_from_link((list)->prev->prev);        \
             wlc_output_get_link(item) != (list);                   \
             item = tmp,                                            \
             tmp = wlc_output_from_link(wlc_output_get_link(item)->prev))

#define wlc_view_for_each(item, list)                               \
        for (item = wlc_view_from_link((list)->next);               \
             wlc_view_get_link(item) != (list);                     \
             item = wlc_view_from_link(wlc_view_get_link(item)->next))

#define wlc_view_for_each_reverse(item, list)                       \
        for (item = wlc_view_from_link((list)->prev);               \
             wlc_view_get_link(item) != (list);                     \
             item = wlc_view_from_link(wlc_view_get_link(item)->prev))

#define wl_view_for_each_safe(item, tmp, list)                      \
        for (item = wlc_view_from_link((list)->next),               \
             tmp = wlc_view_from_link((list)->next->next);          \
             wlc_view_get_link(item) != (list);                     \
             item = tmp,                                            \
             tmp = wlc_view_from_link(wlc_view_get_link(item)->next))

#define wl_view_for_each_safe_reverse(item, tmp, list)              \
        for (item = wlc_view_from_link((list)->prev),               \
             tmp = wlc_view_from_link((list)->prev->prev);          \
             wlc_view_get_link(item) != (list);                     \
             item = tmp,                                            \
             tmp = wlc_view_from_link(wlc_view_get_link(item)->prev))

#define wlc_view_for_each_user(item, list)                           \
        for (item = wlc_view_from_user_link((list)->next);           \
             wlc_view_get_user_link(item) != (list);                 \
             item = wlc_view_from_user_link(wlc_view_get_user_link(item)->next))

#define wlc_view_for_each_user_reverse(item, list)                   \
        for (item = wlc_view_from_user_link((list)->prev);           \
             wlc_view_get_user_link(item) != (list);                 \
             item = wlc_view_from_user_link(wlc_view_get_user_link(item)->prev))

#define wl_view_for_each_user_safe(item, tmp, list)                 \
        for (item = wlc_view_from_user_link((list)->next),          \
             tmp = wlc_view_from_user_link((list)->next->next);     \
             wlc_view_get_user_link(item) != (list);                \
             item = tmp,                                            \
             tmp = wlc_view_from_user_link(wlc_view_get_user_link(item)->next))

#define wl_view_for_each_user_safe_reverse(item, tmp, list)         \
        for (item = wlc_view_from_user_link((list)->prev),          \
             tmp = wlc_view_from_user_link((list)->prev->prev);     \
             wlc_view_get_user_link(item) != (list);                \
             item = tmp,                                            \
             tmp = wlc_view_from_user_link(wlc_view_get_user_link(item)->prev))

#endif /* _WLC_H_ */
