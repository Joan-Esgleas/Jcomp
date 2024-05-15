.POSIX:
.SUFFIXES:

include config.mk

# flags for compiling
JCOMPCPPFLAGS = -I. -DWLR_USE_UNSTABLE -D_POSIX_C_SOURCE=200809L -DVERSION=\"$(VERSION)\" $(XWAYLAND)
JCOMPDEVCFLAGS = -g -pedantic -Wall -Wextra -Wdeclaration-after-statement -Wno-unused-parameter -Wshadow -Wunused-macros\
	-Werror=strict-prototypes -Werror=implicit -Werror=return-type -Werror=incompatible-pointer-types -Wfloat-conversion

# CFLAGS / LDFLAGS
PKGS      = wlroots wayland-server xkbcommon libinput $(XLIBS)
JCOMPCFLAGS = `$(PKG_CONFIG) --cflags $(PKGS)` $(JCOMPCPPFLAGS) $(JCOMPDEVCFLAGS) $(CFLAGS)
LDLIBS    = `$(PKG_CONFIG) --libs $(PKGS)` $(LIBS)

all: jcomp
jcomp: jcomp.o util.o
	$(CC) jcomp.o util.o $(LDLIBS) $(LDFLAGS) $(JCOMPCFLAGS) -o $@
jcomp.o: jcomp.c config.mk config.h client.h cursor-shape-v1-protocol.h pointer-constraints-unstable-v1-protocol.h wlr-layer-shell-unstable-v1-protocol.h xdg-shell-protocol.h
util.o: util.c util.h

# wayland-scanner is a tool which generates C headers and rigging for Wayland
# protocols, which are specified in XML. wlroots requires you to rig these up
# to your build system yourself and provide them in the include path.
WAYLAND_SCANNER   = `$(PKG_CONFIG) --variable=wayland_scanner wayland-scanner`
WAYLAND_PROTOCOLS = `$(PKG_CONFIG) --variable=pkgdatadir wayland-protocols`

cursor-shape-v1-protocol.h:
	$(WAYLAND_SCANNER) server-header \
		$(WAYLAND_PROTOCOLS)/staging/cursor-shape/cursor-shape-v1.xml $@
pointer-constraints-unstable-v1-protocol.h:
	$(WAYLAND_SCANNER) server-header \
		$(WAYLAND_PROTOCOLS)/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml $@
wlr-layer-shell-unstable-v1-protocol.h:
	$(WAYLAND_SCANNER) server-header \
		protocols/wlr-layer-shell-unstable-v1.xml $@
xdg-shell-protocol.h:
	$(WAYLAND_SCANNER) server-header \
		$(WAYLAND_PROTOCOLS)/stable/xdg-shell/xdg-shell.xml $@

config.h:
	cp config.def.h $@
clean:
	rm -f jcomp *.o *-protocol.h

install: jcomp
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f jcomp $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/jcomp
	mkdir -p $(DESTDIR)$(DATADIR)/wayland-sessions
	cp -f jcomp.desktop $(DESTDIR)$(DATADIR)/wayland-sessions/jcomp.desktop
	chmod 644 $(DESTDIR)$(DATADIR)/wayland-sessions/jcomp.desktop
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/jcomp $(DESTDIR)$(DATADIR)/wayland-sessions/jcomp.desktop

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CPPFLAGS) $(JCOMPCFLAGS) -c $<
