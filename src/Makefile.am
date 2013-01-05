AUTOMAKE_OPIONS=foreign no-dependencies

AM_CFLAGS = -D_POSIX

bin_PROGRAMS = iwar2 iwar2_serial

iwar2_SOURCES = iwar2.c \
iwar2-strlcpy.c \
iwar2-strlcat.c \
iwar2-util.c \
iwar2-ncurses.c \
iwar2-config.c  \
iwar2-threads.c \
iwar2-usage.c

iwar2_serial_SOURCES = dialers/serial/iwar_serial.c \
dialers/serial/sysdep1.c

install-data-local:

