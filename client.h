#include "output_monitor.h"

typedef enum  { XDGShell, LayerShell, X11 }client_Type;
typedef struct {

  client_Type
      type; // XDGShell or X11 or LayerShell or whatever whould be in the future
  Monitor *mon;

  struct wlr_scene_tree *scene;
  struct wlr_scene_tree *scene_surface;

  union {
    struct wlr_xdg_surface *xdg;
    struct wlr_xwayland_surface *xwayland;
  } surface;

  struct wl_list link;  // link to have a XDGShell client list
  struct wl_list flink; // link to have a LayerShell client list

  struct wlr_scene_rect *border[4]; /* top, bottom, left, right */
  struct wlr_box geom;
  struct wlr_box prev; /* layout-relative, includes border */
  struct wlr_box bounds;

  struct wlr_xdg_toplevel_decoration_v1 *decoration;
  struct wl_listener set_decoration_mode;
  struct wl_listener destroy_decoration;

  struct wl_listener fullscreen;
  struct wl_listener set_title;
  struct wl_listener destroy;
  struct wl_listener unmap;
  struct wl_listener maximize;
  struct wl_listener map;
  struct wl_listener commit;

#ifdef XWAYLAND
  struct wl_listener activate;
  struct wl_listener associate;
  struct wl_listener dissociate;
  struct wl_listener configure;
  struct wl_listener set_hints;
#endif

  unsigned int bw;
  uint32_t tags;
  int isfloating, isurgent, isfullscreen;
  uint32_t resize; /* configure serial of a pending resize */

} Client;
