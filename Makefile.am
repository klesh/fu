SUBDIRS = fu

EXTRA_DIST = \
	$(desktop_DATA) \
	$(autostart_DATA) \
	$(pixmaps_DATA) 

gtk_update_icon_cache = gtk-update-icon-cache -f -t
install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache


update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache) $(datadir)/icons/hicolor; \
	else \
		echo "*** Icon cache not updated.  After (un)install, run these:"; \
		echo "***   $(gtk_update_icon_cache) $(datadir)/icons/hicolor"; \
	fi
