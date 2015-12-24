#! /usr/bin/awk -f
#
#       (c) 2004 Alexander Perry <alex@pamurray.com>
#                       Licensed under the GPL version 2

BEGIN {
        blank="";
        rows=-1;
        cols=0;
        pending="";
        hadnonblank=0;
        headlines=0;
        taillines=0;
        collected=0;
}

# Check the first and second row are valid for PBMPLAIN

(rows==-1) {
        if ($0 != "P1") {
                print "Not a PBMPLAIN file magic string" > "/dev/stderr";
                exit;
        }
        print $0 > "/dev/stderr";
        rows=0;
        next;
}

(rows==0) {
        rows=$2;
        cols=$1;
        if ( (NF!=2) || (rows<1) ) {
                print "Not PBMPLAIN dimensions" > "/dev/stderr";
                exit;
        }
        next;
}

# Concatenate input lines to accumulate a whole bitmap row

1 {
        partial = $0;
        gsub ( " ", "", partial );
        pending = pending partial;
        if ( length(pending) < cols ) next;
        $0 = pending;
        pending = "";
}

# Find out what a blank line looks like (for speed)

(blank=="") {
        yup = 1;
        for ( i=1; i<=length(blank); i++ ) {
                if ( substr ( $0, i, 1 ) == "0" ) {
                        yup = 0;
                }
        }
        if (yup) {
                blank = $0;
        }
}

# Skip blank lines (may have to add them back)

($0==blank) {
        if ( hadnonblank ) {
                taillines ++;
        } else {
                headlines ++;
        }
        next;
}

# Add line to accumulation (with pending blanks)

1 {
        for ( i=0; i<taillines; i++ ) {
                collected ++;
                collection[collected] = blank;
        }
        taillines = 0;
        hadnonblank = 1;
        collected ++;
        collection[collected] = $0;
}

# End of file reading; everything else happens at end

END {
        rows = 64;

        # Omit unwanted white columns

        lhs = cols;
        rhs = 1;
        for ( r=1; r<=collected; r++ ) {
                s = collection[r];
                for ( i=lhs; i>=1; i-- ) {
                        if ( substr ( s, i, 1 ) != "0" ) {
                                lhs = i;
                        }
                }
                for ( i=rhs; i<=cols; i++ ) {
                        if ( substr ( s, i, 1 ) != "0" ) {
                                rhs = i;
                        }
                }
        }
        cols = rhs-lhs+1;
        for ( r=1; r<=collected; r++ ) {
                s = collection[r];
                collection[r] = substr ( s, lhs, cols );
        }
        blank = substr ( blank, lhs, cols );

        # Make sure we have the correct line count

        while ( collected < rows ) {
                # First add a line on the end
                collected ++;
                collection[collected] = blank;
                # If not enough, one on the front
                if ( collected < rows ) {
                        collected ++;
                        for ( i=collected; i>1; i-- ) {
                                collection[i] = collection[i-1];
                        }
                        collection[1] = blank;
                }
        }

        # Check this is now valid and report it

        print cols " " rows > "/dev/stderr";

        # First provide standard error reference version

        for ( i=1; i<=collected; i++ ) {
                print collection[i] > "/dev/stderr";
        }

        # Now emit the binary output format

        for ( c=1; c<=cols; c++ ) {
            for ( r=1; r<=rows; r+=8 ) {
                n = 0;
                for ( i=r+7; i>=r; i-- ) {
                        z = substr ( collection[i], c, 1 );
                        n = n * 2 + ( z=="1" ? 1 : 0 );
                }
                printf ( "%c", n );
            }
        }

        # End of execution
}