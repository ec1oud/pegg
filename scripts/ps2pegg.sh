#! /bin/bash

# For 9mm tape, 64 pixel height, resolution is at least 180 dpi !

echo "(c) Alex@PAMurray.com" |\
enscript -B -f Times-Roman24 -o - |\
/etc/magicfilter/psonly300-filter |\
tee ps2pegg.ps |\
gs -sDEVICE=pbm -r200x200 -sOutputFile=- -sPAPERSIZE=letter \
        -dNOPAUSE -dSAFER -q - |\
pnmtoplainpnm |\
awk -f ps2pegg.awk 2> ps2pegg.pnm > ps2pegg.pegg

# We've generated the tape, so emit it
./pegg -d2 -w2048 ps2pegg.pegg 
