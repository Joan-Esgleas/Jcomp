#include "keyboard.h"
#include "client.h"


enum {
  LyrBg,
  LyrBottom,
  LyrTile,
  LyrFloat,
  LyrTop,
  LyrFS,
  LyrOverlay,
  LyrBlock,
  NUM_LAYERS
}; /* scene layers */

static const int layermap[] = {LyrBg, LyrBottom, LyrTop, LyrOverlay};

typedef struct  {
  //Core utils
  struct wl_display *display;
  struct wlr_backend *backend;
  struct wlr_renderer *renderer;
  struct wlr_allocator *allocator;
  struct wlr_compositor *compositor;
  struct wlr_session *session;

  //Scene utils
  struct wlr_scene *scene;
  struct wlr_scene_rect *root_bg;
  struct wlr_scene_tree *layers[NUM_LAYERS];
  struct wlr_scene_output_layout *scene_layout;

  //Surface
  struct wlr_surface *held_grab;
  struct wl_list fstack; /* focus order */
  void *exclusive_focus;

  //Xdg Surface
  struct wlr_scene_tree *drag_icon;
  struct wlr_xdg_shell *xdg_shell;
  struct wlr_xdg_activation_v1 *activation;
  struct wlr_xdg_decoration_manager_v1 *xdg_decoration_mgr;
  struct wl_listener new_xdg_toplevel;
  struct wl_listener new_xdg_popup;
  struct wl_list toplevels;

  //Layer Surface
  struct wlr_layer_shell_v1 *layer_shell;

  //Ilde Stuff
  struct wlr_idle_notifier_v1 *idle_notifier;
  struct wlr_idle_inhibit_manager_v1 *idle_inhibit_mgr;
  //Session locked manager
  struct wlr_session_lock_manager_v1 *session_lock_mgr;
  struct wlr_scene_rect *locked_bg;
  struct wlr_session_lock_v1 *cur_lock;
  struct wl_listener lock_listener;
  int locked;

  //Output stuff
  struct wlr_output_manager_v1 *output_mgr;
  struct wlr_output_layout *output_layout;
  struct wlr_box sgeom;
  struct wl_list mons;
  Monitor *selmon;

  //Gamma control manager
  struct wlr_gamma_control_manager_v1 *gamma_control_mgr;

  //Input stuff
  struct wlr_seat *seat;

  struct wlr_virtual_pointer_manager_v1 *virtual_pointer_mgr;
  struct wlr_pointer_constraints_v1 *pointer_constraints;
  struct wlr_relative_pointer_manager_v1 *relative_pointer_mgr;
  struct wlr_pointer_constraint_v1 *active_constraint;

  struct wlr_cursor *cursor;
  struct wlr_xcursor_manager *cursor_mgr;
  struct wlr_cursor_shape_manager_v1 *cursor_shape_mgr;
  unsigned int cursor_mode;

  struct wlr_virtual_keyboard_manager_v1 *virtual_keyboard_mgr;
  KeyboardGroup vkb_group;
  KeyboardGroup kb_group;

  //Client stuff
  Client *grabc;
  int grabcx, grabcy; /* client-relative */

  //XWAYLAND stuff
#ifdef XWAYLAND
  void activatex11(struct wl_listener *listener, void *data);
  void associatex11(struct wl_listener *listener, void *data);
  void configurex11(struct wl_listener *listener, void *data);
  void createnotifyx11(struct wl_listener *listener, void *data);
  void dissociatex11(struct wl_listener *listener, void *data);
  xcb_atom_t getatom(xcb_connection_t *xc, const char *name);
  void sethints(struct wl_listener *listener, void *data);
  void xwaylandready(struct wl_listener *listener, void *data);
  struct wlr_xwayland *xwayland;
  xcb_atom_t netatom[NetLast];
#endif
}Server;
