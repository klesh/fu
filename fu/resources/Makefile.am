desktopdir = $(datadir)/applications
desktop_DATA = fu.desktop

autostartdir = $(sysconfdir)/xdg/autostart
#autostart_in_file = fu_startup.desktop.in
#autostart_DATA = $(autostart_in_file:.desktop.in=.desktop)
autostart_DATA = fu_startup.desktop

#pixmapsdir = $(datarootdir)/icons/hicolor/48x48/apps
pixmapsdir = $(ICONS_PREFIX)

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

gtk_update_icon_cache = gtk-update-icon-cache -f -t
install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache

update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache) $(ICONS_PREFIX); \
	else \
		echo "*** Icon cache not updated.  After (un)install, run these:"; \
		echo "***   $(gtk_update_icon_cache) $(ICONS_PREFIX)"; \
	fi
