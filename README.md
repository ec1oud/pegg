This is a collection of utilities to enable printing to various USB Casio label
printers such as the KL-P1000 and KL-E11. The KP-C10 was the original target
and is egg-shaped, which inspired the name.

The main program is `pegg`, which finds your printer on the USB bus and sends
raw-formatted label files to it.

But you will need some help to generate the raw bitmap files. scripts/xbm2crw
can convert an XBM file to the raw format. You should first create an XBM file
whose width is 64 pixels and height is as long as you want your label to be.

If you install netpbm and ghostscript and imagemagick, you can write a
Postscript file to format the label, use magick to generate an XBM file, use
xbm2crw to generate the raw file and finally send that to the printer.
scripts/peggps does this.  The convention is that one Postscript point = 1
pixel (even though that's not really to scale), so that you can think in pixels
when designing the label.

If you also have a web server on the machine to which the printer is connected
(such as [lighttpd](https://wiki.archlinux.org/title/Lighttpd#CGI) or 
[apache](https://wiki.archlinux.org/title/Apache_HTTP_Server)), you can use the
files under "web" to set up a convenient service for printing labels from
anywhere on your own LAN.  The web server will need to be set up to handle CGI
scripts, and to have an 
[alias](https://redmine.lighttpd.net/projects/lighttpd/wiki/Mod_alias) mapping 
`tmp/` to `/tmp/http/`.  Of course it is possible to customize those scripts to
generate custom labels with extra decorations, multiple text fields in various
places, etc.

Alternatively you can set up the printer with a CUPS driver so that it's
available as a network printer, and use any program you like to send Postscript
to it.  (But you'd better limit the "page" size to 512 x 64 "points".)

