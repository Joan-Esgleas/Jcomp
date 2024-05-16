/*
 * See LICENSE file for copyright and license details.
 */
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include <wayland-server-core.h>

#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_control_v1.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_drm.h>
#include <wlr/types/wlr_export_dmabuf_v1.h>
#include <wlr/types/wlr_fractional_scale_v1.h>
#include <wlr/types/wlr_primary_selection_v1.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_screencopy_v1.h>
#include <wlr/types/wlr_single_pixel_buffer_v1.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_viewporter.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_output_layout.h>

#include "config.h"
#include "server.h"
#include "util.h"

static Server main_server;
pid_t child_pid = -1;

void quit(void *data) { wl_display_terminate(main_server.display); }

void handlesig(int signo) {
  if (signo == SIGCHLD) {
#ifdef WAYLAND
    // TODO: XWAYLAND implementation
#else
    while (waitpid(-1, NULL, WNOHANG) > 0)
      ;

#endif /* ifdef WAYLAND */
  } else if (signo == SIGINT || signo == SIGTERM)
    quit(NULL);
}

void setup(void) {

  // Create display
  main_server.display = wl_display_create();

  // Create backend
  if (!(main_server.backend =
            wlr_backend_autocreate(main_server.display, &main_server.session)))
    die("backend not created");
  // Create scene
  main_server.scene = wlr_scene_create();
  main_server.root_bg =
      wlr_scene_rect_create(&main_server.scene->tree, 0, 0, rootcolor);
  for (int i = 0; i < NUM_LAYERS; ++i)
    main_server.layers[i] = wlr_scene_tree_create(&main_server.scene->tree);
  main_server.drag_icon = wlr_scene_tree_create(&main_server.scene->tree);
  wlr_scene_node_place_below(&main_server.drag_icon->node,
                             &main_server.layers[LyrBlock]->node);

  // Create renderer
  if (!(main_server.renderer = wlr_renderer_autocreate(main_server.backend)))
    die("renderer not created");
  wlr_renderer_init_wl_shm(main_server.renderer, main_server.display);

  if (wlr_renderer_get_dmabuf_texture_formats(main_server.renderer)) {
    wlr_drm_create(main_server.display, main_server.renderer);
    wlr_scene_set_linux_dmabuf_v1(
        main_server.scene, wlr_linux_dmabuf_v1_create_with_renderer(
                               main_server.display, 4, main_server.renderer));
  }
  // Create allocator
  if (!(main_server.allocator = wlr_allocator_autocreate(
            main_server.backend, main_server.renderer))) {
    die("can't create allocator");
  }

  // TODO: create compositor
  main_server.compositor =
      wlr_compositor_create(main_server.display, 6, main_server.renderer);

  // Create wlr_set interfaces
  wlr_subcompositor_create(main_server.display);
  wlr_data_device_manager_create(main_server.display);
  wlr_data_control_manager_v1_create(main_server.display);
  wlr_export_dmabuf_manager_v1_create(main_server.display);
  wlr_screencopy_manager_v1_create(main_server.display);
  wlr_primary_selection_v1_device_manager_create(main_server.display);
  wlr_viewporter_create(main_server.display);
  wlr_single_pixel_buffer_manager_v1_create(main_server.display);
  wlr_fractional_scale_manager_v1_create(main_server.display, 1);

  // TODO: gama color_mgr bind handler

  // TODO: output layoud creation handler
  main_server.output_layout = wlr_output_layout_create();

  // TODO: new outputs abilable handler

  // TODO: Create a list of window clients

  // TODO: new xdg_shell surface handler

  // TODO: new layer_shell handler

  // TODO: ilde manager

  // TODO: session lock mgr

  // TODO: decoration implementation

  // TODO: pointer constraints relative pointer

  // TODO: cursor stuff

  // TODO: keyboard stuff

  // TODO: touchscreen stuff

  // TODO: seat stuff

  // TODO: keyboard stuff

  // TODO: virtual stuff

  // TODO: output manager handlers

  // TODO: start presentation

  // TODO: XWAYLAND stuff
}

void run(char *startup_cmd) {
  // TODO: start server socket

  // TODO: start backend

  /* Run startup command
   */
  if (startup_cmd) {
    int piperw[2];
    if (pipe(piperw) < 0)
      die("startup: pipe:");
    if ((child_pid = fork()) < 0)
      die("startup: fork:");
    if (child_pid == 0) {
      dup2(piperw[0], STDIN_FILENO);
      close(piperw[0]);
      close(piperw[1]);
      execl("/bin/sh", "/bin/sh", "-c", startup_cmd, NULL);
      die("startup: execl:");
    }
    dup2(piperw[1], STDOUT_FILENO);
    close(piperw[1]);
    close(piperw[0]);
  }

  // TODO: set cursor

  // TODO: run display
}

void cleanup(void) {
  // TODO: cleanup XWAYLAND stuff
#ifdef XWAYLAND
  wlr_xwayland_destroy(xwayland);
  xwayland = NULL;
#endif

  wl_display_destroy_clients(main_server.display);
  if (child_pid > 0) {
    kill(child_pid, SIGTERM);
    waitpid(child_pid, NULL, 0);
  }
  wlr_xcursor_manager_destroy(main_server.cursor_mgr);
  wlr_output_layout_destroy(main_server.output_layout);

  /* Remove event source that use the dpy event loop before destroying dpy */
  wl_event_source_remove(main_server.kb_group.key_repeat_source);
  wl_event_source_remove(main_server.vkb_group.key_repeat_source);

  wl_display_destroy(main_server.display);
  /* Destroy after the wayland display (when the monitors are already destroyed)
     to avoid destroying them with an invalid scene output. */
  wlr_scene_node_destroy(&main_server.scene->tree.node);
}

int main(int argc, char *argv[]) {
  char *startup_cmd = NULL;
  int c;

  while ((c = getopt(argc, argv, "s:hdv")) != -1) {
    if (c == 's')
      startup_cmd = optarg;
    else if (c == 'd')
      log_level = WLR_DEBUG;
    else
      goto usage;
  }
  if (optind < argc)
    goto usage;

  /* Wayland requires XDG_RUNTIME_DIR for creating its communications socket */
  if (!getenv("XDG_RUNTIME_DIR"))
    die("XDG_RUNTIME_DIR must be set");

  int sig[] = {SIGCHLD, SIGINT, SIGTERM, SIGPIPE};
  struct sigaction sa = {.sa_flags = SA_RESTART, .sa_handler = handlesig};
  sigemptyset(&sa.sa_mask);

  for (int i = 0; i < (int)LENGTH(sig); i++)
    sigaction(sig[i], &sa, NULL);

  wlr_log_init(log_level, NULL);

  setup();
  run(startup_cmd);
  cleanup();
  return EXIT_SUCCESS;

usage:
  die("Usage: %s [-v] [-d] [-s startup command]", argv[0]);
}
