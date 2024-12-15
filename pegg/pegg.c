/*
	Pegg 0.23 - Linux support for Casio label printers.

	Copyright (c) 2003,2004 Daniel Amkreutz, <daniel.amkreutz@tu-harburg.de>
			  (c) 2004 Alex Perry, <alex@pamurray.com>

	IMPORTANT NOTICE:

	* This software was designed using data made available by and is
	  released with the permission of CASIO COMPUTER CO., LTD.
	* CASIO COMPUTER Co., LTD. assumes no responsibility for the content of
	  this software.
	* Please do not contact CASIO COMPUTER CO., LTD. with any inquiries
	  concerning this software.

	A large amount of code is borrowed from the libusb Documentation.
	The communication API of the Printer has been greatly documented
	and published by CASIO COMPUTER CO., LTD

													Many Thanks.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	You may use this program for whatever you want as long the above written
	text remains.

	FOR KP-C50:

	If you want to test pegg with the KP-C50, some modifications may be
	needed as described in comments.

	Please support Pegg by testing it and reporting bugs.
*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <usb.h>

int _interfacenumber,					   /* Interface and Endpoints of the printer*/
	_endpoint_r, _endpoint_w, density = 3, /* Set the default density */
	label_length = 2,					   /* Set the default length to 512px*/
	width = 512;

/* If this is not null; we're just going to omit a PNM file */
char *pnm_filename = NULL;

char _read_buffer[8], /* Read and Write buffer */
	_write_buffer[64];

char ESC = 0x1b,			/* The Printer's contol codes */
	STX = 0x02, EOT = 0x04, /* Refer to the Command Specs by Casio */
	ENQ = 0x05, ACK = 0x06, NAK = 0x15, SYN = 0x16, CAN = 0x18, EOS = 0x1e, WIT = 0x1f;

char _B = 0x42,			  /* The Printer's command codes I */
	_F = 0x46, _G = 0x47, /* Refer to the Command Specs by Casio */
	_I = 0x49, _N = 0x4e, _P = 0x50, _T = 0x54;

struct usb_bus *busses;
struct usb_bus *bus;
struct usb_device *dev;
usb_dev_handle *_eggprinter;

int chr2int(char s[])
{
	int i, n;
	n = 0;
	for (i = 0; isdigit(s[i]); ++i) {
		n = 10 * n + (s[i] - '0');
	}
	return n;
}

int write_to_printer(int bytes)
{ /* Function to write the date in _write_buffer to the printer */

	/* Printer accepts only Packets site of 1,2,16,64 bytes */

	if (bytes == 1 || bytes == 2 || bytes == 16 || bytes == 64) {

		if (usb_bulk_write(_eggprinter, _endpoint_w, (void *)_write_buffer, bytes, 500) == bytes) {
			return 0;
			bzero(_write_buffer, 64);
		}
		return -2; /* Error code -1 = wrong size */
		bzero(_write_buffer, 64);
	}
	return -1; /* Error code -2 = transmission failed */
	bzero(_write_buffer, 64);
}

int recv_from_printer(int bytes)
{ /* Function to read data from the printer and
	 write it to _read_buffer */

	int n, rd = 0, loop = 0;
	bzero(_read_buffer, 8);

	while (rd < bytes && loop < 10) { /* Read date in a loop, in order to handle
										 delays between transmission */

		n = usb_bulk_read(_eggprinter, _endpoint_r, (void *)_read_buffer, bytes, 50);

		if (n == 0) {
			usleep(200000);
			continue;
		}

		if (n <= 0) {
			printf("USB_BULK_READ ERROR ! Use strace for more information. \n");
			return -1;
		}

		rd += n;
		loop++;
	}

	return n;
}

int scan_usb(void)
{ /* This function scans the USB and sets up the Printer
	 communication */

	struct usb_config_descriptor *cdesc;
	struct usb_interface *interface;
	struct usb_interface_descriptor *idesc;
	struct usb_endpoint_descriptor *epdesc;

	usb_init();
	usb_find_busses();
	usb_find_devices();

	busses = usb_get_busses();

	for (bus = busses; bus; bus = bus->next) { /* Scanning through all Busses and Devices */
		for (dev = bus->devices; dev; dev = dev->next) {

			if (dev->descriptor.idVendor == 0x07cf) {

				if (dev->descriptor.idProduct == 0x4003 || dev->descriptor.idProduct == 0x4001) {

					/* You may want to add the ProductId for the KP-C50 like the folowing:
					   (Replace 0xXXXX with the ProductId)
					   if (dev->descriptor.idProduct == 0x4003 ||
					   dev->descriptor.idProduct == 0xXXXX) {
					*/

					cdesc = dev->config; /* Setting all necessariy device variables */
					interface = cdesc->interface;
					idesc = interface->altsetting;
					epdesc = idesc->endpoint;

					_endpoint_w = epdesc->bEndpointAddress; /* Writing Endpoint  */

					epdesc = &idesc->endpoint[1];			/* Switch endpoint to get the ... */
					_endpoint_r = epdesc->bEndpointAddress; /* ...reading EndPoint */

					_eggprinter = usb_open(dev);

					if (usb_claim_interface(_eggprinter, _interfacenumber) < 0) {
						printf("ERROR: Could not open Interface! Make sure you have the "
							   "appopriate rights. \n");
						return -1;
					}
					return 0;
				}
			}
		}
	}

	return 1;
}

int open_printer()
{

	int found;

	found = scan_usb();
	printf("Scanning USB ...\n");

	if (found == 0) {
		printf("Found Casio KL/KP label printer.\n");
	}

	if (found != 0) {
		printf("No device(s) found. - Maybe uhci.o module not loaded ?\n\n");
		return -1;
	}

	return 0;
}

int close_printer()
{ /* Closes the Printer */

	usb_release_interface(_eggprinter, _interfacenumber);
	usb_reset(_eggprinter); /* This reset is due to some problems with users of SuSe Linux */
	usb_close(_eggprinter);
	printf("Printer closed. \n\n");
	return 0;
}

int display_printer_status()
{

	int ret = 0; /* For communication scheme look at the Casio Command Spec */

	ret = open_printer();

	if (ret < 0) {
		return -1;
	}

	_write_buffer[0] = ESC; /* The Readbuffer is cleared automatically before reading */
	_write_buffer[1] = _I;

	write_to_printer(2);
	recv_from_printer(1);

	_write_buffer[0] = ENQ;

	write_to_printer(1);
	recv_from_printer(8);

	printf("Status Packet request answer: \n\n");
	printf("5: %X  Product-Type 1 = 0x01\n", _read_buffer[4]);
	printf("6: %X  Product-Type 2 =", _read_buffer[5]);

	if (_read_buffer[5] == 0x26) {
		printf(" 0x26 => 0x0126 (KLP1000 / KL-E11)\n");
	}

	if (_read_buffer[5] == 0x27) {
		printf(" 0x27 => 0x0127 (KP-C50)\n");
	}

	if (_read_buffer[5] == 0x28) {
		printf(" 0x28 => 0x0128 (KP-C10)\n");
	}

	printf("7: %X  Version Information\n", _read_buffer[6]);
	printf("8: %X  Support Information\n\n", _read_buffer[7]);

	_write_buffer[0] = ACK;

	write_to_printer(1);
	recv_from_printer(1);

	if (_read_buffer[0] == EOT) {
		printf("Transfer complete.\n");
	}
	close_printer();
	return 0;
}

int send_raw_data(char printdata[])
{ /* The printing routine */

	int ready = 0, /* Have a look at CASIO's command spec for the communication protocol*/
		ret = 0, volume = 0, size = width * 8, i = 0, n = 0, z = 0, datapos = 0;

	char KLP_KPC = 0x00;

	ret = open_printer();

	if (ret < 0) {
		return -1;
	}

	printf("Checking printer type... ");
	_write_buffer[0] = ESC;
	_write_buffer[1] = _I;

	write_to_printer(2);
	recv_from_printer(1);

	_write_buffer[0] = ENQ;

	write_to_printer(1);
	recv_from_printer(8);
	/* Get Printer Type */
	KLP_KPC = _read_buffer[5];

	_write_buffer[0] = ACK;

	write_to_printer(1);
	recv_from_printer(1);

	if (_read_buffer[0] == EOT) {
		printf("OK\n");
	} else {
		printf("ERROR\n");
		return -1;
	}

	if (KLP_KPC == 0x26) { /* Send ESC_F to complete communication for KL-P1000 */

		printf("Checking for paper type... ");
		_write_buffer[0] = ESC;
		_write_buffer[1] = _F;

		write_to_printer(2);
		recv_from_printer(1);

		_write_buffer[0] = ENQ;

		write_to_printer(1);
		recv_from_printer(8);

		printf("%X ", _read_buffer[5]);

		_write_buffer[0] = ACK;

		write_to_printer(1);
		recv_from_printer(1);

		if (_read_buffer[0] == EOT) {
			printf("OK\n");
		} else {
			printf("ERROR\n");
			return -1;
		}
	}

	_write_buffer[0] = ESC;
	_write_buffer[1] = _P;

	printf("Print format notification... ");

	write_to_printer(2);
	recv_from_printer(1);

	if (_read_buffer[0] == ACK) {
		printf("OK\n");
	} else {
		printf("ERROR\n");
		return -1;
	}

	_write_buffer[0] = STX; /* Necessary epilogue created before printing */
	_write_buffer[1] = 0x80;
	_write_buffer[2] = 0x0c;

	_write_buffer[7] = density;
	_write_buffer[8] = 0x40; /* Height 64Dots */
	_write_buffer[9] = 0x00;

	if (KLP_KPC == 0x26) { /* IF KL-P1000 */

		_write_buffer[10] = (width % 256);
		_write_buffer[11] = (width / 256);
		_write_buffer[12] = (size % 256); /* Transfer data size */
		_write_buffer[13] = (size / 256);

	} else { /* FIXED FOR KP Series */

		_write_buffer[10] = 0x02; /* 0x0200 = 512 dots width */
		_write_buffer[11] = 0x00; /* Transfer data size 4096 Bytes*/
		_write_buffer[12] = 0x10;
		_write_buffer[13] = 0x00;
	}

	printf("Sending print format... ");

	write_to_printer(16);
	recv_from_printer(1);

	if (_read_buffer[0] == ACK) {
		printf("OK\n");
	} else {
		printf("ERROR\n");
		return -1;
	}

	printf("Picture transfer request... ");

	_write_buffer[0] = ESC;
	_write_buffer[1] = _G;

	write_to_printer(2);
	recv_from_printer(1);

	if (_read_buffer[0] == ACK) {
		printf("OK\n");
	} else {
		printf("ERROR\n");
		return -1;
	}

	printf("DATA TRANSFER ");

	if ((size % 1024) == 0) {
		volume = size / 1024;
	} else {
		volume = size / 1024 + 1;
	}

	for (i = 0; i <= volume; i++) { /* Number of pages */

		_write_buffer[0] = STX;
		_write_buffer[1] = 0x00;
		_write_buffer[2] = 0x00;
		_write_buffer[3] = 0x04;

		if (i == volume) { /* Last packet */
			_write_buffer[1] = 0x80;
			if (volume % 1024 != 0) {
				_write_buffer[2] = (volume % 1024) % 256;
				_write_buffer[3] = (volume % 1024) / 256;
			}
		}

		for (n = 0; n < 16; n++) { /* Each page has 17 Blocks with 64kByte */

			if (n == 0) {

				for (z = 0; z < 60; z++) {
					_write_buffer[z + 4] = printdata[datapos + z];
				}
				datapos = datapos + z;
				write_to_printer(64);

			} else {

				for (z = 0; z < 64; z++) {
					_write_buffer[z] = printdata[datapos + z];
				}
				datapos = datapos + z;
				write_to_printer(64);
			}
		}

		for (z = 0; z < 4; z++) {
			_write_buffer[z] = printdata[datapos + z];
		}

		datapos = datapos + z;

		write_to_printer(64);
		recv_from_printer(1);

		if (_read_buffer[0] == ACK) {
			printf("...%dkB", i);
			fflush(stdout);
		} else {
			printf("ERROR\n");
			return -1;
		}
	}

	_write_buffer[0] = ESC;
	_write_buffer[1] = _N;
	write_to_printer(2);

	recv_from_printer(1);
	if (_read_buffer[0] == ACK) {
		printf(" OK\n");
	} else {
		printf(" ERROR\n");
		return -1;
	}

	if (KLP_KPC == 0x26) { /* Send ESC_F to complete communication for KL-P1000 */

		printf("Waiting for printer");
		fflush(stdout);

		do {

			sleep(1);

			_write_buffer[0] = ESC;
			_write_buffer[1] = _T;
			write_to_printer(2);
			recv_from_printer(1);

			if (_read_buffer[0] == WIT) {
				printf(".");
				fflush(stdout);
			} else {

				if (_read_buffer[0] == ACK) {
					printf(" OK\n");
					ready = 1;

					printf("Tape feed... ");
					_write_buffer[0] = 0xfe;
					write_to_printer(1);
					recv_from_printer(1);

					if (_read_buffer[0] == ACK) {
						printf("OK\n");
					} else {
						printf("ERROR\n");
						return -1;
					}
				}
			}
		} while (ready == 0);
	}

	ready = 0;
	sleep(2);
	printf("Status request ");
	fflush(stdout);

	do {
		sleep(1);

		_write_buffer[0] = ESC;
		_write_buffer[1] = _B;
		write_to_printer(2);
		recv_from_printer(1);
		if (_read_buffer[0] == ACK) {
			ready = 1;
			printf(" OK\n");
		} else {
			printf(".");
			fflush(stdout);
		}

	} while (ready == 0);

	printf("Closing printer...\n");
	close_printer();
	return 0;
}

int main(int argc, char *argv[])
{ /* The Main Program handles commandline args
	 and processes the image-file */

	int test = 0, c = 0, i = 0;
	FILE *raw_file;

	printf("%s - %s, %s\n", "Pegg Version 0.23", "(c) 2003,04 Daniel Amkreutz", "(c) 2004 Alex Perry\n");

	if (argc < 2) {
		printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n", "WRONG NUMBER OF ARGUMENTS.\n", "Usage:\n\tpegg -w (-d) (-t) (-o) PEGG-FILE\n",
			   "Options:", "\t-d<dens>\tset print density 0=light...4=dark (eg. -d3)", "\t-t\t\tprint Information about device",
			   "\t-o<file>\tExport to <file> (PNM) instead of printing", "\t-w<width>\twidth in px (eg. -w512) only KL-P1000");
		exit(0);
	}

	/* Handle command line argunemts */
	for (i = 0; i < argc; i++) {

		if (strstr(argv[i], "-w") != 0x00) {
			if (strtok(argv[i], "-w") != NULL)
				width = chr2int(strtok(argv[i], "-w"));
		}
		if (strstr(argv[i], "-d") != 0x00) {
			if (strtok(argv[i], "-d") != NULL)
				density = chr2int(strtok(argv[i], "-d"));
		}
		if (strstr(argv[i], "-o") != 0x00) {
			if (strtok(argv[i], "-o") != NULL)
				pnm_filename = 2 + strstr(argv[i], "-o");
		}
		if (strstr(argv[i], "-t") != 0x00)
			test = 1;
	}

	/* Deal with the test special case */
	if (test != 0) {
		display_printer_status();
		exit(0);
	}

	/* We know how big the raw file will be */
	char data[(width * 8 - 1) + 1024];
	bzero(data, width * 8 + 1024);
	/* Not testing, so find our input file */

	raw_file = fopen(argv[i - 1], "rb"); /* raw-file is the LAST arg so it can be read here */

	if (raw_file == NULL) {
		printf("Error opening file: %s !\n", argv[i - 1]);
		exit(-1);
	}
	/* Read it and close it */
	for (i = 0; i < (width * 8); i++) {
		c = getc(raw_file);
		data[i] = c;
		if (c < 0)
			width = (i / 8);
	}

	fclose(raw_file);
	printf("%i Bytes read for width %i.\n", i, width);

	/* Normally we simply call the printout routine */
	if (pnm_filename == NULL) {
		send_raw_data(data);
		exit(0);
	}
	/* Now open the dump file */
	printf("*%s*\n", pnm_filename);
	raw_file = fopen(pnm_filename, "wb");
	if (raw_file == NULL) {
		perror("Failed to open pnm output file");
		exit(-2);
	}

	/* This generates PNMPLAIN format output */
	fprintf(raw_file, "P1 %i 64\n", width);
	for (i = 0; i < 64; i++)
		for (c = 0; c < width; c++)
			fprintf(raw_file, "%i%c", 0 != (((unsigned int)data[(c * 8) + (i / 8)]) & (((unsigned int)1) << (i % 8))),
					((c == width - 1)) ? '\n' : ' ');
	fclose(raw_file);

	/* All is well if we got this far */
	exit(0);
}
