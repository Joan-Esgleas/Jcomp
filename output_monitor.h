#include <wayland-server-core.h>
#include <wayland-util.h>
#include <wlr/util/box.h>
#include <wlr/types/wlr_layer_shell_v1.h>
#include <wlr/types/wlr_scene.h>
#include "server.h"

#define LAYER_SURF_XM 4

typedef char Ly_sym;
typedef struct Monitor Monitor;
typedef struct Layout Layout;
typedef struct LayerSurface LayerSurface;

struct Layout {
  const Ly_sym lysym;
  void (*arrange)(Monitor *);
};

struct Monitor {
  struct wl_list link; // Link para hacer una lista de monitores

  Server *server;
  struct wlr_output *wlr_output;
  struct wlr_scene_output *scene_output;

  struct wl_listener frame;
  struct wl_listener destroy;
  struct wl_listener request_state;
  struct wl_listener destroy_lock_surface;

  struct wlr_scene_rect
      *fullscreen_bg; // Posible implementacion para un fondo de pantalla
  struct wlr_session_lock_surface_v1 *lock_surface;

  struct wlr_box m_area; // Monitor area
  struct wlr_box w_area; // Window area

  struct wl_list layers[LAYER_SURF_XM];
  const Layout *lt[2]; // Link related Layouts

  unsigned int seltags;
  unsigned int sellt;
  uint32_t tagset[2];

  float mfact;
  int gamma_lut_changed;
  int nmaster;
  Ly_sym ltsymbol[16];
};

struct LayerSurface {
  struct wlr_box geom;
  Monitor *mon;
  
  struct wlr_scene_tree *scene;
  struct wlr_scene_tree *popups;
  
  struct wlr_scene_layer_surface_v1 *scene_layer;
  struct wl_list link;
  int mapped;
  struct wlr_layer_surface_v1 *layer_surface;
  

  struct wl_listener destroy;
  struct wl_listener map;
  struct wl_listener unmap;
  struct wl_listener surface_commit;
};




void arrange(Server *server,Monitor *m);
void arrangelayer(Server *server,Monitor *m, struct wl_list *list,
                          struct wlr_box *usable_area, int exclusive);
void arrangelayers(Server *server,Monitor *m);
