TARGET ?= zbitxd
OWNER ?= $(TARGET)
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
STATEDIR ?= /var/lib/$(OWNER)
SHAREDIR ?= $(PREFIX)/share/$(OWNER)
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
HEADERS = $(wildcard *.h)
CFLAGS = -I. -O3 -march=native -mtune=native -fomit-frame-pointer -fstrict-aliasing
LIBS = -lwiringPi -lasound -lm -lfftw3 -lfftw3f -pthread -lncurses -lsqlite3 -lsystemd ft8_lib/libft8.a
ifdef SBITX_UNUSED
## remove and print unused code
CFLAGS += -ffunction-sections -fdata-sections
LIBS += -Wl,--gc-sections,--print-gc-sections
endif
ifdef SBITX_DEBUG
CFLAGS += -ggdb3 -fsanitize=address
LIBS += -fsanitize=address
endif
CC = gcc
LINK = gcc
STRIP = strip

$(TARGET): $(OBJECTS) ft8_lib/libft8.a
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBPATH) $(LIBS)

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(SOURCES:.c=.d)

%.o: %.c
	$(CC) -c $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) -o $@ $<

configure.h: create_configure.h

create_configure.h:
	$(shell echo "#define STATEDIR \"$(STATEDIR)\"" > configure.h) 
	$(shell echo "#define SHAREDIR \"$(SHAREDIR)\"" >> configure.h) 

ft8_lib/libft8.a:
ifdef SBITX_DEBUG
	$(MAKE) FT8_DEBUG=1 -C ft8_lib
else
	$(MAKE) -C ft8_lib
endif

clean:
	-rm -f configure.h
	-rm -f $(OBJECTS)
	-rm -f *~ core *.core
	-rm -f $(TARGET)
	$(MAKE) -C ft8_lib clean

adduser:
	-adduser --system --group --home $(DESTDIR)/$(STATEDIR) --disabled-password $(OWNER)
	-adduser $(OWNER) audio
	-adduser $(OWNER) gpio

install: adduser
	install -D --mode=755 $(TARGET) $(DESTDIR)/$(BINDIR)/$(TARGET)
	install -d --owner=$(OWNER) --group=$(OWNER) $(DESTDIR)/$(SHAREDIR)/web
	install -m 644 --owner=$(OWNER) --group=$(OWNER) web/* $(DESTDIR)/$(SHAREDIR)/web
	install -d --owner=$(OWNER) --group=$(OWNER) $(DESTDIR)/$(STATEDIR)
	install -m 644 --owner=$(OWNER) --group=$(OWNER) data/default_hw_settings.ini $(DESTDIR)/$(STATEDIR)
	install -m 644 --owner=$(OWNER) --group=$(OWNER) data/default_settings.ini $(DESTDIR)/$(STATEDIR)
	install -d $(DESTDIR)/$(PREFIX)/lib/systemd/system/
	install -m 644 systemd/zbitxd.service $(DESTDIR)/$(PREFIX)/lib/systemd/system
	ln -sf /var/lib/zbitxd/grids.txt /usr/local/share/zbitxd/web/grids.txt
ifeq ("$(wildcard $(DESTDIR)/$(STATEDIR)/sbitx.db)","")
	$(shell sqlite3 $(DESTDIR)/$(STATEDIR)/sbitx.db < data/create_db.sql)
endif

uninstall:
	rm -f $(DESTDIR)/$(BINDIR)/$(TARGET)
	rm -rf $(DESTDIR)/$(SHAREDIR)
	rm -f $(DESTDIR)/$(STATEDIR)/default_hw_settings.ini
	rm -f $(DESTDIR)/$(STATEDIR)/default_settings.ini
	rm -f $(DESTDIR)/$(PREFIX)/lib/systemd/system/zbitxd.service

.PHONY: adduser create_configure.h clean install uninstall
