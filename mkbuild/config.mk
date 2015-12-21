arch = x86_64
nprocs = 2

TOOLCHAIN_TRIPLET = ${arch}-musl-linux
CC = ${TOOLCHAIN_TRIPLET}-gcc
HOSTCC = $CC -static
LD = $CC
AR = ${TOOLCHAIN_TRIPLET}-ar
RANLIB = ${TOOLCHAIN_TRIPLET}-ranlib
STRIP = ${TOOLCHAIN_TRIPLET}-strip
#STRIP = true
SHELL = /bin/sh

PREFIX =
BINDIR = ${PREFIX}/bin
LIBDIR = ${PREFIX}/lib
ETCDIR = ${PREFIX}/etc
DFLDIR = ${ETCDIR}/default
MANDIR = ${PREFIX}/share/man

INSTALL = /usr/bin/install
SUM = sha512sum

CPPFLAGS = -D_BSD_SOURCE -D_GNU_SOURCE
CFLAGS = ${CPPFLAGS}
LDFLAGS = -static
