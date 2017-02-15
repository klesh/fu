SUBDIRS = core os protocols ui
bin_PROGRAMS = fu
fu_LDADD = core/libcore.a \
					 os/libos.a \
					 protocols/imgur/libimgur.a \
					 protocols/local/liblocal.a \
					 protocols/sftp/libsftp.a \
					 protocols/smms/libsmms.a \
					 protocols/libprotocols.a \
					 ui/libui.a
fu_SOURCES = main.cpp
#at_LDADD = $(LIBOBJS)

