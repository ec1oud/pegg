# Makefile for pegg
#
# Here you should check the configuration parameters for pegg
# Please make sure that the paths are correct

# CONFIGURATION:
# -------------

# Installation path (the path where the executable will be installed):
PREFIX=/usr/local/bin

# The path for the manpage (normally /usr/share/man)
MAN_PATH=/usr/local/share/man

# Compiler flags (default: display all warnings)
CFLAGS=-Wall

# Enter the path to the usb.h include file. If you do not know the path
# you might try the folowing command: find /usr -name usb.h
INCLUDE_PATH=/usr/include

# Here you should give the path to the usb-library (libusb.so etc)
# If you are not sure about the path then use: find /usr -name libusb*
LIB_PATH=/usr/lib


# DO NOT CHANGE ANYTÄHING BELOW HERE
# ----------------------------------

pegg:	pegg.c
	gcc -o pegg pegg.c ${CFLAGS} -I${INCLUDE_PATH} -L${LIB_PATH} -lusb

all:	pegg

install:
	cp pegg ${PREFIX}/.
	gzip -c pegg.1 > ${MAN_PATH}/man1/pegg.1.gz

clean:
	rm pegg

uninstall:
	rm ${PREFIX}/pegg
	rm ${MAN_PATH}/man1/pegg.1.gz


