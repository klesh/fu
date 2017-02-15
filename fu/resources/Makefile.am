desktopdir = $(datadir)/applications
desktop_DATA = fu.desktop

autostartdir = $(sysconfdir)/xdg/autostart
#autostart_in_file = fu_startup.desktop.in
#autostart_DATA = $(autostart_in_file:.desktop.in=.desktop)
autostart_DATA = fu_startup.desktop

pixmapsdir = $(datarootdir)/icons/hicolor/48x48/apps
pixmaps_DATA = fu.png fu_uploading.png

EXTRA_DIST = \
  $(desktop_DATA) \
  $(autostart_DATA) \
  $(pixmaps_DATA)

CLEANFILES = \
  $(desktop_DATA) \
  $(autostart_DATA)

DISTCLEANFILES = \
  $(desktop_DATA) \
  $(autostart_DATA)
