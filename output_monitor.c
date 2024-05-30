#include "output_monitor.h"
#include "client.h"
#include <string.h>
#include <wayland-util.h>
#include "util.h"


void arrange(Server *server, Monitor *m) {
  Client *c;

  if(!m->wlr_output->enabled)
    return;

  wl_list_for_each(c, &server->clients, link) {
    if(c->mon == m) {
      wlr_scene_node_set_enabled(&c->scene->node, VISIBLEON(c, m));
      client_set_suspended(c, !VISIBLEON(c,m));
    }
  }

  //TODO: keep track of the upmost client
  wlr_scene_node_set_enabled(&m->fullscreen_bg->node, 
                             c->isfullscreen);

  strncpy(m->ltsymbol, &m->lt[m->sellt]->lysym, LENGTH(m->ltsymbol));

  if(m->lt[m->sellt]->arrange)
    m->lt[m->sellt]->arrange(m);

  
}

void arrangelayer(Server *server, Monitor *m, struct wl_list *list,
                         struct wlr_box *usable_area, int exclusive) {
  LayerSurface *l;
  struct wlr_box full_area = m->m_area;

  wl_list_for_each(l, list, link) {
    struct wlr_layer_surface_v1 *layer_surface = l->layer_surface;

    if (exclusive != (layer_surface->current.exclusive_zone > 0))
      continue;

    wlr_scene_layer_surface_v1_configure(l->scene_layer, &full_area,
                                         usable_area);

    wlr_scene_node_set_position(&l->popups->node, l->scene->node.x,
                                l->scene->node.y);
    l->geom.x = l->scene->node.x;
    l->geom.y = l->scene->node.y;
  }
}

void arrangelayers(Server *server, Monitor *m) {

  struct wlr_box usable_area = m->m_area;
  LayerSurface *l;
  uint32_t layers_above_shell[] = {
      ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY,
      ZWLR_LAYER_SHELL_V1_LAYER_TOP,
  };
  if (!m->wlr_output->enabled)
    return;

  for (int i = LAYER_SURF_XM - 1; i >= 0; i--) {
    arrangelayer(server, m, &m->layers[i], &usable_area, 1);

    if (!wlr_box_equal(&usable_area, &m->w_area)) {
      m->w_area = usable_area;
      arrange(server,m);
    }
  }

  for (int i = LAYER_SURF_XM - 1; i >= 0; i--) {
    arrangelayer(server, m, &m->layers[i], &usable_area, 0);
  }

  //TODO: keyboard handling
}
