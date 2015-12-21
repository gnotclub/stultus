include config.mk

.POSIX:
.SUFFIXES: .c .o

LIB = \
	common.o  \
	db.o      \
	ealloc.o  \
	eprintf.o \
	pkg.o     \
	reject.o  \
	strlcat.o \
	strlcpy.o

SRC = \
	infopkg.c    \
	installpkg.c \
	removepkg.c

SHPROG = \
	fetchpkg     \
	searchpkg

OBJ = $(SRC:.c=.o) $(LIB)
BIN = $(SRC:.c=)

all: binlib

binlib: util.a
	$(MAKE) bin

bin: $(BIN)

$(OBJ): pkg.h config.mk

.o:
	@echo LD $@
	@$(LD) -o $@ $< util.a $(LDFLAGS)

.c.o:
	@echo CC $<
	@$(CC) -c -o $@ $< $(CFLAGS)

util.a: $(LIB)
	@echo AR $@
	@$(AR) -r -c $@ $(LIB)

install: all
	@echo installing executables to $(DESTDIR)$(PREFIX)/bin
	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@cp -f $(BIN) $(DESTDIR)$(PREFIX)/bin
	@cp -f $(SHPROG) $(DESTDIR)$(PREFIX)/bin
	@for i in $(SHPROG); do chmod 755 $(DESTDIR)$(PREFIX)/bin/$$i; done

uninstall:
	@echo removing executables from $(DESTDIR)$(PREFIX)/bin
	@cd $(DESTDIR)$(PREFIX)/bin && rm -f $(BIN) $(SHPROG)

clean:
	@echo cleaning
	@rm -f $(BIN) $(OBJ) $(LIB) util.a
