include ../Makefile.inc

applicationsdir = $(DESKTOP_DATADIR)/applications
applications_DATA = aegisub.desktop

iconsscalabledir = $(DESKTOP_DATADIR)/icons/hicolor/scalable/apps
iconsscalable_DATA = scalable/aegisub.svg


all:

install-data-hook:
	touch "$(DESTDIR)$(DESKTOP_DATADIR)/icons/hicolor"

ICON_HICOLOR = \
	16x16/aegisub.png \
	22x22/aegisub.png \
	24x24/aegisub.png \
	32x32/aegisub.png \
	48x48/aegisub.png \
	64x64/aegisub.png

ICON_SCALABLE = \
	scalable/aegisub.svg

DESKTOP_FILE = \
	aegisub.desktop

EXTRA_DIST = \
	aegisub.desktop.in \
	$(ICON_HICOLOR) \
	$(ICON_SCALABLE)

install:
	@$(BIN_MKDIR) -p $(P_DESKTOP)
	$(BIN_INSTALL) $(DESKTOP_FILE) $(P_DESKTOP)/$(DESKTOP_FILE)
	@$(BIN_MKDIR) -p $(addprefix $(P_ICON)/, $(dir $(ICON_HICOLOR)))
	@for i in $(ICON_HICOLOR); do \
		$(BIN_ECHO) "$(BIN_INSTALL) $$i $(P_ICON)/$$i"; \
		$(BIN_INSTALL) $$i $(P_ICON)/$$i; \
	done



CLEANFILES = aegisub.desktop

include ../Makefile.target
