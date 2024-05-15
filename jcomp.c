/*
 * See LICENSE file for copyright and license details.
 */
#include <getopt.h>
#include <libinput.h>
#include <linux/input-event-codes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/backend/libinput.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_cursor_shape_v1.h>
#include <wlr/types/wlr_data_control_v1.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_drm.h>
#include <wlr/types/wlr_export_dmabuf_v1.h>
#include <wlr/types/wlr_fractional_scale_v1.h>
#include <wlr/types/wlr_gamma_control_v1.h>
#include <wlr/types/wlr_idle_inhibit_v1.h>
#include <wlr/types/wlr_idle_notify_v1.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_keyboard_group.h>
#include <wlr/types/wlr_layer_shell_v1.h>
#include <wlr/types/wlr_linux_dmabuf_v1.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_output_management_v1.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_pointer_constraints_v1.h>
#include <wlr/types/wlr_presentation_time.h>
#include <wlr/types/wlr_primary_selection.h>
#include <wlr/types/wlr_primary_selection_v1.h>
#include <wlr/types/wlr_relative_pointer_v1.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_screencopy_v1.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_server_decoration.h>
#include <wlr/types/wlr_session_lock_v1.h>
#include <wlr/types/wlr_single_pixel_buffer_v1.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_viewporter.h>
#include <wlr/types/wlr_virtual_keyboard_v1.h>
#include <wlr/types/wlr_virtual_pointer_v1.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_xdg_activation_v1.h>
#include <wlr/types/wlr_xdg_decoration_v1.h>
#include <wlr/types/wlr_xdg_output_v1.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/log.h>
#include <wlr/util/region.h>
#include <xkbcommon/xkbcommon.h>
#ifdef XWAYLAND
#include <wlr/xwayland.h>
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#endif

#include "config.h"
#include "util.h"


//TODO: implement this
void handlesig(int signo); 

void setup(void) {
  //TODO: create display
  
  //TODO: create backend  

  //TODO: create scene
    
  //TODO: create renderer

  //TODO: create compositor
  
  //TODO: create wlr_set interfaces
  
  //TODO: gama color_mgr bind handler

  //TODO: output layoud creation handler

  //TODO: new outputs abilable handler

  //TODO: Create a list of window clients
  
  //TODO: new xdg_shell surface handler

  //TODO: new layer_shell handler 
    
  //TODO: ilde manager

  //TODO: session lock mgr
  
  //TODO: decoration implementation
  
  //TODO: pointer constraints relative pointer

  //TODO: cursor stuff

  //TODO: keyboard stuff

  //TODO: touchscreen stuff

  //TODO: seat stuff

  //TODO: keyboard stuff

  //TODO: virtual stuff

  //TODO: output manager handlers

  //TODO: start presentation

  //TODO: XWAYLAND stuff
}

void run(char *startup_cmd) {
  //TODO: start server socket

  //TODO: start backend

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

  //TODO: set cursor
  
  //TODO: run display
}

void cleanup(void) {
  //TODO: cleanup XWAYLAND stuff

  //TODO: cleanup dpy

  //TODO: cleanup childs

  //TODO: cleanup cursor

  //TODO: cleanup output layout

  //TODO: cleanup keyboard

  //TODO: cleanup display

  //TODO: cleanup scene
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

  // TODO: implement handlesig
  int i, sig[] = {SIGCHLD, SIGINT, SIGTERM, SIGPIPE};
  struct sigaction sa = {.sa_flags = SA_RESTART, .sa_handler = handlesig};
  sigemptyset(&sa.sa_mask);

  for (i = 0; i < (int)LENGTH(sig); i++)
    sigaction(sig[i], &sa, NULL);

  wlr_log_init(log_level, NULL);

  setup();
  run(startup_cmd);
  cleanup();
  return EXIT_SUCCESS;

usage:
  die("Usage: %s [-v] [-d] [-s startup command]", argv[0]);
}
