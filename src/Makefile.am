AUTOMAKE_OPIONS=foreign no-dependencies

AM_CFLAGS = -D_POSIX

bin_PROGRAMS = iwar2 iwar2-serial

iwar2_SOURCES = iwar2.c \
iwar2-strlcpy.c \
iwar2-strlcat.c \
iwar2-ncurses.c \
iwar2-util.c \
iwar2-config.c  \
iwar2-threads.c \
iwar2-usage.c

iwar2_serial_SOURCES = dialers/serial/iwar2-serial.c \
dialers/serial/iwar2-serial-usage.c \
dialers/serial/iserial-util.c \
dialers/serial/sysdep1.c \
iwar2-strlcpy.c \
iwar2-strlcat.c \
iwar2-util.c

install-data-local:

